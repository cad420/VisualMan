#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include <cstring> // memcpy
#include <transformation.h>
#include <arithmetic.h>
#include <vector>
#include <unordered_map>

#include "shape.h"
#include "material.h"
#include "interaction.h"

#include "materialreader.h"

namespace ysl
{
	class TriangleMesh
	{
		std::unique_ptr<Point3f[]> m_vertices;
		std::unique_ptr<Vector3f[]> m_normals;
		std::unique_ptr<Point2f[]> m_textures;
		std::vector<int> m_vertexIndices;
		const int m_nTriangles;
		const int m_nVertex;
		ysl::Bound3f m_bound;

	public:
		TriangleMesh(const Transform & objectToWorld,
			const Point3f * vertices,
			const Vector3f * normals,
			const Point2f * textures,
			int nVertex,
			const int * vertexIndices,
			int nTriangles)noexcept :
			m_nVertex(nVertex),
			m_nTriangles(nTriangles),
			m_vertexIndices(vertexIndices, vertexIndices + 3 * nTriangles)
		{
			m_vertices.reset(new ysl::Point3f[nVertex]);
			for (int i = 0; i < nVertex; i++)
			{
				m_vertices[i] = objectToWorld * vertices[i];
				//objectToWorld += m_vertices[i];
			}
			//m_centroid /= nVertex;

			int normalBytes = 0;
			int textureBytes = 0;
			if (normals != nullptr) {
				//const auto nm = trans.normalMatrix();
				const auto nm = objectToWorld.Matrix().NormalMatrix();
				normalBytes = m_nVertex * sizeof(ysl::Vector3f);
				m_normals.reset(new ysl::Vector3f[nVertex]);
				for (int i = 0; i < nVertex; i++) {
					//m_normals[i] = trans * normals[i];
					const auto x = normals[i].x, y = normals[i].y, z = normals[i].z;
					m_normals[i] = ysl::Vector3f{ x*nm.m[0][0] + y * nm.m[0][1] + z * nm.m[0][2],x*nm.m[1][0] + y * nm.m[1][1] + z * nm.m[1][2],x*nm.m[2][0] + y * nm.m[2][1] + z * nm.m[2][2] }.Normalized();
				}
			}
			if (textures != nullptr) {
				textureBytes = m_nVertex * sizeof(ysl::Point2f);
				m_textures.reset(new ysl::Point2f[nVertex]);
				std::memcpy(m_textures.get(), textures, nVertex * sizeof(ysl::Point2f));
			}


		}
		const Point3f * Vertices()const
		{
			return m_vertices.get();
		}
		int VertexCount()const
		{
			return m_nVertex;
		}
		const int * Indices()const
		{
			return m_vertexIndices.data();
		}
		int TriangleCount()const
		{
			return m_nTriangles;
		}
		const Vector3f *Normals()const
		{
			return m_normals.get();
		}

		const Point2f *Textures()const
		{
			return m_textures.get();
		}
		//void transform(const Transform & trans)
		//{
		//	for (int i = 0; i < m_nTriangles; i++)
		//	{
		//		m_vertices[i] = trans * m_vertices[i];
		//	}
		//}
		friend class Triangle;
	};

	class Triangle :public Shape {
		std::shared_ptr<TriangleMesh> m_sharedTriangles;
		const int * m_vertexIndices;
	public:
		Triangle(std::shared_ptr<TriangleMesh> mesh, int indexOffset, std::shared_ptr<Material> m = nullptr) :Shape(m)
		{
			m_sharedTriangles = mesh;
			m_vertexIndices = &(mesh->m_vertexIndices[indexOffset * 3]);
		}
		Bound3f bound()const override
		{
			return Bound3f(m_sharedTriangles->m_vertices[m_vertexIndices[0]],
				m_sharedTriangles->m_vertices[m_vertexIndices[1]]).UnionWith(
					m_sharedTriangles->m_vertices[m_vertexIndices[2]]);
		}
		Float area()const override
		{
			const auto v1 = m_sharedTriangles->m_vertices[m_vertexIndices[1]]
				- m_sharedTriangles->m_vertices[m_vertexIndices[0]];
			const auto v2 = m_sharedTriangles->m_vertices[m_vertexIndices[2]]
				- m_sharedTriangles->m_vertices[m_vertexIndices[0]];
			return 0.5*Vector3f::Dot(v1, v2);
		}

		Interaction sample(const Point2f & u) const override 
		{
			Point2f s = uniformSampleTriangle(u);
			Point3f p0 = m_sharedTriangles->m_vertices[m_vertexIndices[0]];
			Point3f p1 = m_sharedTriangles->m_vertices[m_vertexIndices[1]];
			Point3f p2 = m_sharedTriangles->m_vertices[m_vertexIndices[2]];

			Point3f sp = p0 * s[0] + p1 * s[1] + (1 - s[0] - s[1])*p2;

			Vector3f p0Norm = m_sharedTriangles->m_normals[m_vertexIndices[0]];
			Vector3f p1Norm = m_sharedTriangles->m_normals[m_vertexIndices[1]];
			Vector3f p2Norm = m_sharedTriangles->m_normals[m_vertexIndices[2]];

			Interaction isect;
			//parameter of the triangle
			isect.m_u = u[0];
			isect.m_v = u[1];

			///TODO::
			//surface normal
			isect.m_norm = (u[0] * p1Norm + u[1] * p2Norm + (1 - u[0] - u[1])*p0Norm);
			//shape
			isect.m_pShape = this;
			//point
			isect.m_p = sp;
			//error bound
			return isect;
		}

		bool intersect(const Ray & ray, Float * t, Interaction * interac)const override;


		static std::vector<std::shared_ptr<Shape>>
			createTriangleMesh(const Point3f * vertices, const Vector3f * normals,const Point2f * textures, int nVertex,
				const int * vertexIndices, int nIndex, std::unordered_map<int, std::string> & mtlName, MaterialReader & mtlReader,
				std::vector<std::shared_ptr<Shape>>* lightShapes,
				const Transform & trans)
		{
			assert(nIndex % 3 == 0);
			if (lightShapes != nullptr)
				lightShapes->clear();
			std::shared_ptr<TriangleMesh> mesh(new TriangleMesh(trans,vertices,
				normals,
				textures,
				nVertex,
				vertexIndices,
				nIndex/3
				));
			std::vector<std::shared_ptr<Shape>> tris;
			for (int i = 0; i < nIndex / 3; i++)
			{
				tris.push_back(std::make_shared<Triangle>(mesh, i));
				std::string name = mtlName[i];
				//qDebug() << QString::fromStdString(name);
				MaterialType type;
				if (name == "sphere_transmission") 
				{
					type = MaterialType::Glass;
				}
				else if (name == "sphere_mirror") 
				{
					type = MaterialType::Mirror;
				}
				else 
				{
					type = MaterialType::Metal;
				}

				RGBASpectrum kd = mtlReader[name]["Kd"];
				RGBASpectrum ks = mtlReader[name]["Ks"];
				RGBASpectrum ka = mtlReader[name]["Ka"];
				RGBASpectrum tf = mtlReader[name]["Tf"];
				RGBASpectrum niV = mtlReader[name]["Ni"];
				RGBASpectrum nsV = mtlReader[name]["Ns"];
				RGBASpectrum ke = mtlReader[name]["Ke"];
				if (kd.IsNull()) {
					kd = RGBASpectrum(0.0);
				}
				if (ks.IsNull()) {
					ks = RGBASpectrum(0.0f);
				}
				if (ka.IsNull()) {
					ka = RGBASpectrum(0.f);
				}
				if (tf.IsNull()) {
					tf = RGBASpectrum(0.f);
				}
				if (ke.IsNull()) {
					ke = RGBASpectrum(0.f);
				}
				else {
					//light source
					if (lightShapes != nullptr)
						lightShapes->push_back(tris.back());
				}
				Float ni;
				if (niV.IsNull()) 
				{
					ni = 0;
				}
				else {
					ni = niV[0];
				}
				Float ns;
				if (nsV.IsNull()) {
					ns = 0;
				}
				else {
					ns = nsV[0];
				}

				///TODO: distiguish different material by MaterialType

				tris.back()->setMaterial(std::make_shared<Material>(kd, ks, ka, ke, tf, ni, ns, type));
			}
			return tris;
		}
	};


	std::shared_ptr<TriangleMesh> CreateTriangleMeshFromFile(const ysl::Transform& objectToWorld, const std::string & fileName);

}

#endif /*_TRIANGLE_H_*/

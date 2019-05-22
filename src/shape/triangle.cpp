
#include "triangle.h"
#include "../gui/transferfunctionwidget.h"
#include "objreader.h"
#include "error.h"

namespace ysl
{
	std::shared_ptr<TriangleMesh> CreateTriangleMeshFromFile(const ysl::Transform& objectToWorld,
		const std::string& fileName)
	{
		ObjReader reader(fileName);

		if (!reader.IsLoaded())
		{
			ysl::Warning("%s cannot be loaded\n", fileName.c_str());
		}

		return std::make_shared<TriangleMesh>(objectToWorld,
			reader.GetVertices().data(),
			reader.GetNormals().data(),
			reader.GetTexCoord().data(),
			reader.GetVertexCount(),
			reader.GetFaceIndices().data(),
			reader.GetFaceIndices().size() / 3);
	}

	bool ysl::Triangle::intersect(const Ray & ray, Float * t, Interaction * interac)const
	{
		/*
		* This ray-triangle intersection algorithm is from
		* "Fast, Minimum Storage Ray-Triangle Intersection"
		* Tomas Moller & Ben Trumbore (1997) Fast, Minimum
		* Storage Ray-Triangle Intersection, Journal of Graphics
		* Tools, 2:1, 21-28, DOI: 10.1080/10867651.1997.10487468
		*
		*/

		const Point3f & p0 = m_sharedTriangles->m_vertices[m_vertexIndices[0]];
		const Point3f & p1 = m_sharedTriangles->m_vertices[m_vertexIndices[1]];
		const Point3f & p2 = m_sharedTriangles->m_vertices[m_vertexIndices[2]];

		const Vector3f & D = ray.d;
		Vector3f T;
		Vector3f E1 = p1 - p0;
		Vector3f E2 = p2 - p0;
		Vector3f P = Vector3f::Cross(D, E2);

		Float det = Vector3f::Dot(P, E1);
		if (det > 0) {
			T = ray.o - p0;
		}
		else {
			T = p0 - ray.o;
			det = -det;
		}
		if (det < 0.000001)
			return false;
		Float u, v;
		//u is the parameter coresponding to p1
		//v is the parameter coresponding to p2
		u = Vector3f::Dot(P, T);
		if (u<0.0 || u>det) {
			//u > 1, invalid
			return false;
		}
		Vector3f Q = Vector3f::Cross(T, E1);
		v = Vector3f::Dot(D, Q);
		if (v < 0.0 || v + u>det) {
			// v > 1 ,invalid
			return false;
		}
		Float tt = Vector3f::Dot(E2, Q);

		Float inv = 1.0f / det;
		tt *= inv;

		if (tt < 0)return false;
		if (tt > ray.tMax)
			return false;
		u *= inv;
		v *= inv;

		//evaluate intersection information
		if (interac != nullptr) {
			interac->m_p = ray.o + ray.d*tt;
			interac->m_u = u;
			interac->m_v = v;

			//a normal of intersction should be interpulated
			const auto p0Norm = m_sharedTriangles->m_normals[m_vertexIndices[0]];
			const auto p1Norm = m_sharedTriangles->m_normals[m_vertexIndices[1]];
			const auto p2Norm = m_sharedTriangles->m_normals[m_vertexIndices[2]];

			const auto norm = (u * p1Norm + v * p2Norm + (1 - v - u)*p0Norm).Normalized();
			interac->m_norm = norm;

			//create two tangent vectors perpendicular to the normal
			const auto t = Vector3f(-norm.z, 0, norm.x).Normalized();
			const auto s = Vector3f::Cross(t, norm).Normalized();
			interac->m_t = t;
			interac->m_s = s;
			interac->m_pShape = this;

			getMaterial()->computeScatteringFunction(interac);
		}
		if (tt < 0.00001)return false;
		if (t)*t = tt;
		return true;
	}
}

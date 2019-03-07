#define TINYOBJLOADER_IMPLEMENTATION
#include <string>
#include <sstream>
#include "materialreader.h"
#include "objreader.h"

#include "../../lib/3rdparty/tiny_obj_loader.h"
#include "error.h"

namespace ysl
{
	ObjReader::ObjReader()noexcept :
		m_loaded(false)
	{
	}

	ObjReader::ObjReader(const std::string & fileName, const std::string & mtlFileName) noexcept :ObjReader()
	{
		m_loaded = Load(fileName);
	}
	bool ObjReader::Load(const std::string & fileName, const std::string & mtlFileName)
	{
		_init();

		//MaterialReader mtlReader(mtlFileName);

		//std::ifstream fileIn(fileName);
		//if (fileIn.is_open() == false) {
		//	std::cerr << "Can not open this file\n";
		//	return (m_loaded = false);
		//}

		//std::string line;
		//char l;
		//int faceCount = 0;

		//std::string prevMtlName;

		//while (getline(fileIn, line)) {
		//	//std::cout << line << std::endl;
		//	line = line.substr(0, line.find_first_of('#'));
		//	//line = line.substr(line.find_first_not_of(' ')+1);
		//	std::stringstream ss(line);
		//	std::string objKeyword;
		//	ss >> objKeyword;
		//	if (objKeyword == "v") {
		//		float x, y, z;
		//		ss >> x >> y >> z;
		//		m_vertices.emplace_back(x, y, z);
		//	}
		//	else if (objKeyword == "vt") {
		//		float x, y;
		//		ss >> x >> y;
		//		m_textures.emplace_back(x, y);
		//	}
		//	else if (objKeyword == "vn") {
		//		float x, y, z;
		//		ss >> x >> y >> z;
		//		m_normals.emplace_back(x, y, z);
		//	}
		//	else if (objKeyword == "usemtl")
		//	{
		//		ss >> prevMtlName;
		//	}
		//	else if (objKeyword == "f") {
		//		std::string str;
		//		std::vector<std::tuple<int, int, int>> indexBuffer;
		//		//arbitray patches
		//		while (ss >> str) {
		//			int vid, vtid, vnid;
		//			sscanf(str.c_str(), "%d/%d/%d", &vid, &vtid, &vnid);
		//			indexBuffer.push_back(std::tuple<int, int, int>(vid - 1, vtid - 1, vnid - 1));
		//		}
		//		//transform to triangle patches
		//		assert(indexBuffer.size() >= 3);
		//		for (int i = 1; i < indexBuffer.size() - 1; i++)
		//		{
		//			m_vertexIndices.emplace_back(std::get<0>(indexBuffer[0]));
		//			m_textureIndices.emplace_back(std::get<1>(indexBuffer[0]));
		//			m_normalIndices.emplace_back(std::get<2>(indexBuffer[0]));
		//			m_vertexIndices.emplace_back(std::get<0>(indexBuffer[i]));
		//			m_textureIndices.emplace_back(std::get<1>(indexBuffer[i]));
		//			m_normalIndices.emplace_back(std::get<2>(indexBuffer[i]));
		//			m_vertexIndices.emplace_back(std::get<0>(indexBuffer[i + 1]));
		//			m_textureIndices.emplace_back(std::get<1>(indexBuffer[i + 1]));
		//			m_normalIndices.emplace_back(std::get<2>(indexBuffer[i + 1]));
		//			if (prevMtlName != std::string())
		//			{
		//				const char * debug = prevMtlName.c_str();
		//				m_indexToMtlName[faceCount] = prevMtlName;
		//			}
		//			faceCount++;
		//		}
		//	}

		//	ss.clear();
		//}
		////if there is no normals in files, normal vectors could be created by cross product of vertices
		//if (m_normals.size() == 0)
		//{
		//	assert(m_vertexIndices.size() % 3 == 0);
		//	m_normals.resize(m_vertices.size());
		//	for (int i = 0; i < m_vertexIndices.size(); i += 3)
		//	{
		//		Vector3f norm = Vector3f::Cross(
		//			m_vertices[m_vertexIndices[i + 1]] - m_vertices[m_vertexIndices[i]],
		//			m_vertices[m_vertexIndices[i + 2]] - m_vertices[m_vertexIndices[i]]);
		//		m_normals[m_vertexIndices[i]] =
		//			m_normals[m_vertexIndices[i + 1]] =
		//			m_normals[m_vertexIndices[i + 2]] = norm;
		//	}
		//}

		//assert(m_vertexIndices.size() == m_normalIndices.size() && m_normalIndices.size() == m_textureIndices.size());
		//int nVertex = m_vertexIndices.size();
		//std::vector<Point3f> vertices(nVertex);
		//std::vector<Vector3f> normals(nVertex);
		//std::vector<Point2f> textures(nVertex);


		//for (int i = 0; i < nVertex; i++) {
		//	vertices[i] = m_vertices[m_vertexIndices[i]];
		//	normals[i] = m_normals[m_normalIndices[i]];
		//	textures[i] = m_textures[m_textureIndices[i]];
		//	m_vertexIndices[i] = i;
		//}
		//m_vertices.swap(vertices);
		//m_normals.swap(normals);
		//m_textures.swap(textures);


		using namespace tinyobj;


		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string warn, err;
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, fileName.c_str()))
			return false;
		if (!err.empty())
			std::cerr << err << std::endl;

		ysl::Log("Number of Shader contained in .Obj: %d\n",shapes.size());


		// reserve

		///TODO:: For better performance, the memory of vectors should be reserved before data wrote in it.

		std::size_t index = 0;

		for(const auto & s:shapes)
		{
			std::size_t indexOffset = 0;
			for(std::size_t f = 0;f<s.mesh.num_face_vertices.size();f++)
			{
				const auto fv = s.mesh.num_face_vertices[f];

				///TODO:: The shape of face should be determined here
				// fv == 3, if it is triangle
				// fv == 4, if it is rectangle

				for(auto v = 0;v<fv;v++)
				{
					index_t idx = s.mesh.indices[indexOffset + v];
					const real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
					const real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
					const real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
					m_vertices.emplace_back(vx, vy, vz);
					if(idx.normal_index != -1)
					{
						const real_t nx = attrib.normals[3 * idx.normal_index + 0];
						const real_t ny = attrib.normals[3 * idx.normal_index + 1];
						const real_t nz = attrib.normals[3 * idx.normal_index + 2];
						m_normals.emplace_back(nx, ny, nz);
					}
					if(idx.texcoord_index != -1)
					{
						const real_t tx = attrib.texcoords[2 * idx.texcoord_index + 0];
						const real_t ty = attrib.texcoords[2 * idx.texcoord_index + 1];
						m_textures.emplace_back(tx, ty);
					}
					m_indices.push_back(index);
					index++;
				}
				indexOffset += fv;
			}
		}

		return (m_loaded = true);
	}


	bool ObjReader::IsLoaded() const
	{
		return m_loaded;
	}
	ObjReader::~ObjReader()
	{
	}

	void ObjReader::_init()
	{
		m_normals.clear();
		m_vertices.clear();
		m_textures.clear();
		m_indices.clear();
		//m_normalIndices.clear();
		//m_textureIndices.clear();
		m_loaded = false;
	}



}

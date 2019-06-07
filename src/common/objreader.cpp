#define TINYOBJLOADER_IMPLEMENTATION
#include <string>
#include <sstream>
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
		Init();

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

	void ObjReader::Init()
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


#ifndef _OBJREADER_H_
#define _OBJREADER_H_
#include <vector>
#include <unordered_map>
#include "../mathematics/geometry.h"

namespace ysl
{

	class ObjReader
	{
	public:
		ObjReader()noexcept;
		ObjReader(const std::string  & fileName, const std::string & mtlFileName = std::string())noexcept;
		bool Load(const std::string & fileName, const std::string & mtlFileName = std::string());
		bool IsLoaded()const;

		std::size_t getVertexCount()const { return m_vertices.size(); }


		std::size_t vertexBytes()const { return m_vertices.size() * sizeof(Point3f); }
		std::size_t normalBytes()const { return m_normals.size() * sizeof(Vector3f); }
		std::size_t textureBytes()const { return m_textures.size() * sizeof(Point2f); }
		std::size_t vertexIndicesBytes()const { return m_indices.size() * sizeof(int); }

		// deprecated
		//std::size_t getNormalCount()const { return m_normals.size(); }

		// deprecated
		//std::size_t getTextureCoordCount()const { return m_textures.size(); }

		
		//std::size_t getFacesCount()const { return m_vertexIndices.size(); }

		const std::vector<Point3f> & getVertices()const { return m_vertices; }

		const std::vector<int> & getFaceIndices()const { return m_indices; }

		const std::vector<Vector3f> & getNormals()const { return m_normals; }

		const std::vector<Point2f> getTextureCoord()const { return m_textures; }


		//const Point3f * getVerticesFlatArray()const { return m_vertices.data(); }
		//const Vector3f * getNormalsFlatArray()const { return m_normals.data(); }
		//const int * getFacesIndicesFlatArray()const { return m_vertexIndices.data(); }

	/*	std::unordered_map<int, std::string> & getIndexToMtlName()
		{
			return m_indexToMtlName;
		}*/
		virtual ~ObjReader();
	private:
		void _init();
	private:
		bool m_loaded;
		std::vector<Vector3f> m_normals;
		std::vector<Point3f> m_vertices;
		std::vector<Point2f> m_textures;
	//	std::unordered_map<int, std::string> m_indexToMtlName;
		std::vector<int> m_indices;
	//	std::vector<int> m_normalIndices;
	//	std::vector<int> m_textureIndices;

	};

}
#endif /*_OBJREADER_H_*/
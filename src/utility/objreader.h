
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
		bool load(const std::string & fileName, const std::string & mtlFileName = std::string());
		bool isLoaded()const;

		std::size_t getVertexCount()const { return m_vertices.size(); }
		std::size_t getNormalCount()const { return m_normals.size(); }
		std::size_t getTextureCoordCount()const { return m_textures.size(); }
		std::size_t getFacesCount()const { return m_vertexIndices.size(); }

		const std::vector<Point3f> & getVertices()const { return m_vertices; }
		const std::vector<int> & getFaceIndices()const { return m_vertexIndices; }
		const std::vector<Vector3f> & getNormals()const { return m_normals; }

		const Point3f * getVerticesFlatArray()const { return m_vertices.data(); }
		const Vector3f * getNormalsFlatArray()const { return m_normals.data(); }
		const int * getFacesIndicesFlatArray()const { return m_vertexIndices.data(); }

		std::unordered_map<int, std::string> & getIndexToMtlName()
		{
			return m_indexToMtlName;
		}
		virtual ~ObjReader();
	private:
		void _init();
	private:
		bool m_loaded;
		std::vector<Vector3f> m_normals;
		std::vector<Point3f> m_vertices;
		std::vector<Point2f> m_textures;
		std::unordered_map<int, std::string> m_indexToMtlName;
		std::vector<int> m_vertexIndices;
		std::vector<int> m_normalIndices;
		std::vector<int> m_textureIndices;

	};

}
#endif /*_OBJREADER_H_*/
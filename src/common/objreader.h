
#ifndef _OBJREADER_H_
#define _OBJREADER_H_
#include <vector>
#include "geometry.h"
#include "common.h"

namespace ysl
{

	class COMMON_EXPORT_IMPORT ObjReader
	{
	public:
		ObjReader()noexcept;

		ObjReader(const std::string  & fileName, const std::string & mtlFileName = std::string())noexcept;
		bool Load(const std::string & fileName, const std::string & mtlFileName = std::string());
		bool IsLoaded()const;

		std::size_t GetVertexCount()const { return m_vertices.size(); }
		std::size_t GetVertexBytes()const { return m_vertices.size() * sizeof(Point3f); }
		std::size_t GetNormalBytes()const { return m_normals.size() * sizeof(Vector3f); }
		std::size_t GetTexCoordBytes()const { return m_textures.size() * sizeof(Point2f); }
		std::size_t GetVertexIndicesBytes()const { return m_indices.size() * sizeof(int); }

		const std::vector<Point3f> & GetVertices()const { return m_vertices; }
		const std::vector<int> & GetFaceIndices()const { return m_indices; }
		const std::vector<Vector3f> & GetNormals()const { return m_normals; }
		const std::vector<Point2f>& GetTexCoord() const { return m_textures; }

		virtual ~ObjReader();
	private:
		void Init();
		bool m_loaded;
		std::vector<Vector3f> m_normals;
		std::vector<Point3f> m_vertices;
		std::vector<Point2f> m_textures;
		std::vector<int> m_indices;

	};



}
#endif /*_OBJREADER_H_*/
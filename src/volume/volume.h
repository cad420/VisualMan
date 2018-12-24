#ifndef VOLUME_H_
#define VOLUME_H_
#include <string>
#include "../memory/blockarray.h"

#include <list>
#include <unordered_map>
#include "volume_utils.h"
#include "lineararray.h"
//#include "../mathematics/arithmetic.h"


struct VolumeFormat;

// Enum Type
enum VoxelType { UInt8, Float32 };
enum VoxelFormat { Grayscale, RGB, RGBA };


struct GlobalBlockAbstractIndex
{
	GlobalBlockAbstractIndex(std::size_t linearId,int xb,int yb,int zb)
	{
		z = linearId / (xb*yb);
		y = (linearId - z * xb*yb) / xb;
		x = linearId - z * xb*yb - y * xb;
	}
	using size_type = int;
	size_type x, y, z;
};


class LargeVolumeCache:public BlockVolumeReader
{
	static constexpr int nLogBlockSize = 5;

	static constexpr size_t cacheBlockCountAtWidth = 20;
	static constexpr size_t cacheBlockCountAtHeight = 20;
	static constexpr size_t cacheBlockCountAtDepth = 20;

	static constexpr size_t m_cacheWidth = cacheBlockCountAtWidth*(1<<nLogBlockSize);
	static constexpr size_t m_cacheHeight = cacheBlockCountAtHeight*(1 << nLogBlockSize);
	static constexpr size_t m_cacheDepth = cacheBlockCountAtDepth*(1 << nLogBlockSize);

	static constexpr size_t totalCacheBlocks = cacheBlockCountAtWidth*cacheBlockCountAtHeight*cacheBlockCountAtDepth;

	using Cache = ysl::Block3DArray<unsigned char, nLogBlockSize>;
	struct LRUListCell;
	using LRUHash = std::unordered_map<int,std::list<LRUListCell>::iterator>;
	struct LRUListCell
	{
		int blockCacheIndex;
		LRUHash::iterator hashIter;
		LRUListCell(int index, LRUHash::iterator itr) :blockCacheIndex{ index }, hashIter{itr}{}
	};
	std::list<LRUListCell> m_lruList;
	LRUHash	m_blockIdInCache;		// blockId---> (blockIndex,the position of blockIndex in list)

	bool m_valid;
	

	std::unique_ptr<Cache> m_volumeCache;

	int blockCoordinateToBlockId(int xBlock, int yBlock, int zBlock)const
	{
		//const auto nxBlock = width() / m_volumeCache.BlockSize(), nyBlock = height() / m_volumeCache.BlockSize(), nzBlock = depth() / m_volumeCache.BlockSize();
		const auto nxBlock = xBlockCount(), nyBlock = yBlockCount(), nzBlock = zBlockCount();
		return zBlock * nxBlock*nyBlock + yBlock * nxBlock + zBlock;
	}


public:
	explicit LargeVolumeCache(const std::string & fileName) :BlockVolumeReader(fileName),m_valid(true)
	{
		if (blockSizeInLog() != nLogBlockSize)
		{
			m_valid = false;
			return;
		}
		m_volumeCache.reset(new Cache(m_cacheWidth, m_cacheHeight, m_cacheDepth, nullptr));
		//
		for (int i = 0; i < totalCacheBlocks; i++)
			m_lruList.push_front(LRUListCell(i,m_blockIdInCache.end()));
	}

	bool valid()const { return m_valid; }

	//int blockCount()const { totalCacheBlocks; }

	constexpr std::size_t cacheBlockCount() const { return totalCacheBlocks; }
	constexpr std::size_t xCacheBlockCount()const { return cacheBlockCountAtWidth; }
	constexpr std::size_t yCacheBlockCount()const { return cacheBlockCountAtHeight; }
	constexpr std::size_t zCacheBlockCount()const { return cacheBlockCountAtDepth; }
	constexpr std::size_t cacheWidth()const { return m_cacheWidth; }
	constexpr std::size_t cacheHeight()const { return m_cacheHeight; }
	constexpr std::size_t cacheDepth()const { return m_cacheDepth; }

	const unsigned char * blockData(int xBlock, int yBlock, int zBlock) { return blockData(blockCoordinateToBlockId(xBlock, yBlock, zBlock)); }
	const unsigned char * blockData(int blockId);
	const unsigned char * blockData(const GlobalBlockAbstractIndex & index){ return blockData(index.x, index.y, index.z); };
};

//class LargeVolumeRepresentation
//{
//	
//};
//
//
//class UnstructuredVolumeRepresentation
//{
//	
//};
//
//class BlockVolumeRepresentation
//{
//	
//};



/**
 * \brief The volume data reader give the basic information about a volume data file,
 * and it provide some method for reading data of different types
 */

 //class VolumeDataReader
 //{
 //	std::ifstream m_fileHandle;
 //	std::string m_fileName;
 //	//std::unique_ptr<char[]> m_buffer;
 //	//std::vector<void> m_pool;
 //public:
 //	VolumeDataReader(const std::string & fileName, const VolumeFormat & fmt);
 //	VolumeDataReader(const VolumeDataReader & reader) = delete;
 //	VolumeDataReader & operator=(const VolumeDataReader & reader) = delete;
 //	VolumeDataReader(VolumeDataReader && reader)noexcept
 //	{
 //		m_fileHandle = std::move(reader.m_fileHandle);
 //		//m_buffer = std::move(reader.m_buffer);
 //	}
 //	VolumeDataReader & operator=(VolumeDataReader && reader)noexcept
 //	{
 //		m_fileHandle = std::move(reader.m_fileHandle);
 //		//m_buffer = std::move(reader.m_buffer);
 //		return *this;
 //	}
 //
 //	std::string fileName()const
 //	{
 //		return m_fileName;
 //	}
 //
 //
 //
 //	//void * wholeData();
 //	//ysl::Vector3i getDimension()const;
 //	//VolumeFormat dataFormat()const;
 //	//void * subData(const ysl::Point3i & min, const ysl::Point3i & max);
 //	//double sampleAt(const ysl::Point3f & pos) const;
 //
 //	//template<typename T>
 //	//std::shared_ptr<VolumeDataRepresentation> volumeData()const
 //	//{
 //	//	
 //	//}
 //
 //	//void readData(VolumeDataRepresentation & repr);
 //
 //
 //
 //	~VolumeDataReader() = default;
 //};
 //
 //






#endif
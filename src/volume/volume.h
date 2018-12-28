#ifndef VOLUME_H_
#define VOLUME_H_
#include <string>
#include "../memory/blockarray.h"

#include <list>
#include <unordered_map>

#include "volume_utils.h"
#include "lineararray.h"

#include "../mathematics/geometry.h"
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
	static constexpr int nLogBlockSize = 6;

	static constexpr ysl::Size3 cacheBlockSize{1<<nLogBlockSize,1<<nLogBlockSize,1<<nLogBlockSize};
	static constexpr ysl::Size3 cacheDim{ 4,4,4 };
	static constexpr ysl::Size3 cacheSize = cacheDim * (1 << nLogBlockSize);

	//static constexpr size_t cacheBlockCountAtWidth = 4;
	//static constexpr size_t cacheBlockCountAtHeight= 4;
	//static constexpr size_t cacheBlockCountAtDepth = 4;

	//static constexpr size_t m_cacheWidth = cacheBlockCountAtWidth*(1<<nLogBlockSize);
	//static constexpr size_t m_cacheHeight = cacheBlockCountAtHeight*(1 << nLogBlockSize);
	//static constexpr size_t m_cacheDepth = cacheBlockCountAtDepth*(1 << nLogBlockSize);

	static constexpr size_t totalCacheBlocks = cacheDim.x*cacheDim.y*cacheDim.z;


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
		const auto nxBlock = xBlockCount(), nyBlock = yBlockCount(), nzBlock = zBlockCount();
		return zBlock * nxBlock*nyBlock + yBlock * nxBlock + xBlock;
	}

public:
	explicit LargeVolumeCache(const std::string & fileName) :BlockVolumeReader(fileName),m_valid(true)
	{
		if (blockSizeInLog() != nLogBlockSize)
		{
			m_valid = false;
			return;
		}

		m_volumeCache.reset(new Cache(cacheSize.x, cacheSize.y, cacheSize.z, nullptr));
		for (auto i =std::size_t(0) ; i < totalCacheBlocks; i++)
			m_lruList.push_front(LRUListCell(i,m_blockIdInCache.end()));
	}

	bool IsValid()const { return m_valid; }

	//int blockCount()const { totalCacheBlocks; }

	constexpr std::size_t CacheBlockCount() const { return totalCacheBlocks; }
	constexpr ysl::Size3 CacheBlockSize()const { return cacheBlockSize; }
	constexpr ysl::Size3 CacheDim()const { return cacheDim; }
	constexpr ysl::Size3 CacheSize()const { return cacheSize; }


	const unsigned char * blockData(int xBlock, int yBlock, int zBlock) { return blockData(blockCoordinateToBlockId(xBlock, yBlock, zBlock)); }
	const unsigned char * blockData(int blockId);
	const unsigned char * blockData(const GlobalBlockAbstractIndex & index){ return blockData(index.x, index.y, index.z); };

};

#endif
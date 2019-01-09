#ifndef VOLUME_H_
#define VOLUME_H_
#include <string>
#include "../memory/blockarray.h"

#include <list>
#include <unordered_map>

#include "volume_utils.h"
#include "lineararray.h"

#include "../mathematics/geometry.h"
#include <mutex>
#include <map>
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


class LargeVolumeCache:public LVDReader
{
	static constexpr int nLogBlockSize = 6;		//

	static constexpr ysl::Size3 cacheBlockSize{1<<nLogBlockSize,1<<nLogBlockSize,1<<nLogBlockSize};
	static constexpr ysl::Size3 cacheDim{10,10,10};
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
	using LRUHash = std::map<int,std::list<LRUListCell>::iterator>;
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

	//ysl::Size3 sizeByBlock;
	//ysl::Size3 cacheSize;
	//ysl::Size3 blockSize;

	std::mutex m_lock;


	int blockCoordinateToBlockId(int xBlock, int yBlock, int zBlock)const
	{
		const auto x = SizeByBlock().x, y = SizeByBlock().y, z = SizeByBlock().z;
		return zBlock * x*y + yBlock * x + xBlock;
	}

	void createCache();

	void initLRU();

public:
	explicit LargeVolumeCache(const std::string& fileName);

	bool IsValid()const { return m_valid; }

	//int blockCount()const { totalCacheBlocks; }

	constexpr std::size_t CacheBlockCount() const { return totalCacheBlocks; }
	constexpr ysl::Size3 CacheBlockSize()const { return cacheBlockSize; }
	constexpr ysl::Size3 CacheDim()const { return cacheDim; }
	constexpr ysl::Size3 CacheSize()const { return cacheSize; }


	const unsigned char * ReadBlockDataFromCache(int xBlock, int yBlock, int zBlock) { return ReadBlockDataFromCache(blockCoordinateToBlockId(xBlock, yBlock, zBlock)); }
	const unsigned char * ReadBlockDataFromCache(int blockId);
	const unsigned char * ReadBlockDataFromCache(const GlobalBlockAbstractIndex & index){ return ReadBlockDataFromCache(index.x, index.y, index.z); };


};

#endif
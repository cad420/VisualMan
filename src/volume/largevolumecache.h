
#ifndef _LARGEVOLUMECACHE_H_
#define _LARGEVOLUMECACHE_H_
#include "geometry.h"
#include <map>
#include <list>
#include "lvdreader.h"

namespace ysl
{


	struct VirtualMemoryBlockIndex
	{
		VirtualMemoryBlockIndex(std::size_t linearId, int xb, int yb, int zb)
		{
			z = linearId / (xb*yb);
			y = (linearId - z * xb*yb) / xb;
			x = linearId - z * xb*yb - y * xb;
		}
		VirtualMemoryBlockIndex(int x, int y, int z):x(x),y(y),z(z)
		{
		}

		using size_type = int;
		size_type x, y, z;
	};

	class CPUCache
	{
	public:
		virtual const unsigned char * ReadBlockDataFromCPUCache(int xBlock, int yBlock, int zBlock) = 0;
		virtual const unsigned char * ReadBlockDataFromCPUCache(int blockId) = 0;
		virtual const unsigned char * ReadBlockDataFromCPUCache(const VirtualMemoryBlockIndex & index) = 0;
		virtual ~CPUCache(){}
	};

	class CPUVolumeDataCache : public ysl::LVDReader,public CPUCache
	{
		static constexpr int nLogBlockSize = 7;		//

		static constexpr ysl::Size3 cacheBlockSize{ 1 << nLogBlockSize,1 << nLogBlockSize,1 << nLogBlockSize };
		static constexpr ysl::Size3 cacheDim{ 16,16,16 };
		static constexpr ysl::Size3 cacheSize = cacheDim * (1 << nLogBlockSize);

		static constexpr size_t totalCacheBlocks = cacheDim.x*cacheDim.y*cacheDim.z;

		using Cache = ysl::Block3DArray<unsigned char, nLogBlockSize>;

		struct LRUListCell;
		using LRUHash = std::map<int, std::list<LRUListCell>::iterator>;
		struct LRUListCell
		{
			int blockCacheIndex;
			LRUHash::iterator hashIter;
			LRUListCell(int index, LRUHash::iterator itr) :blockCacheIndex{ index }, hashIter{ itr }{}
		};
		std::list<LRUListCell> m_lruList;
		LRUHash	m_blockIdInCache;		// blockId---> (blockIndex,the position of blockIndex in list)

		bool m_valid;
		std::unique_ptr<Cache> m_volumeCache;

		//ysl::Size3 sizeByBlock;
		//ysl::Size3 cacheSize;
		//ysl::Size3 blockSize;

		//std::mutex m_lock;


		int blockCoordinateToBlockId(int xBlock, int yBlock, int zBlock)const
		{
			const auto x = SizeByBlock().x, y = SizeByBlock().y, z = SizeByBlock().z;
			return zBlock * x*y + yBlock * x + xBlock;
		}
		void createCache();
		void initLRU();
	public:
		explicit CPUVolumeDataCache(const std::string& fileName);
		bool IsValid()const { return m_valid; }
		//int blockCount()const { totalCacheBlocks; }
		std::size_t CPUCacheBlockCount() const { return totalCacheBlocks; }
		ysl::Size3 CPUCacheBlockSize()const { return cacheBlockSize; }
		ysl::Size3 CPUCacheDim()const { return cacheDim; }
		ysl::Size3 CPUCacheSize()const { return cacheSize; }

		const unsigned char * ReadBlockDataFromCPUCache(int xBlock, int yBlock, int zBlock)override{ return ReadBlockDataFromCPUCache(blockCoordinateToBlockId(xBlock, yBlock, zBlock)); }
		const unsigned char * ReadBlockDataFromCPUCache(int blockId)override;
		const unsigned char * ReadBlockDataFromCPUCache(const VirtualMemoryBlockIndex & index)override { return ReadBlockDataFromCPUCache(index.x, index.y, index.z); };
	};

}


#endif /*_LARGEVOLUMECACHE_H_*/
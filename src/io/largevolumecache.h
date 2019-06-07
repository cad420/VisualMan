
#ifndef _LARGEVOLUMECACHE_H_
#define _LARGEVOLUMECACHE_H_
#include "../common/geometry.h"
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

	class AbstrCPUBlockCache
	{
	public:
		virtual const unsigned char * ReadBlockDataFromCPUCache(int xBlock, int yBlock, int zBlock) = 0;
		virtual const unsigned char * ReadBlockDataFromCPUCache(int blockId) = 0;
		virtual const unsigned char * ReadBlockDataFromCPUCache(const VirtualMemoryBlockIndex & index) = 0;
		virtual Size3 BlockSize() = 0;
		virtual Size3 BlockDim() = 0;
		//virtual std::size_t MemoryBytes()const = 0;
		virtual ~AbstrCPUBlockCache();
	};

	class AbstrBlockedVolumeDataCPUCache:public AbstrCPUBlockCache
	{
		const Size3 cacheBlockDim;
	public:
		AbstrBlockedVolumeDataCPUCache(const Size3 & blockDim) :cacheBlockDim(blockDim) {}
		virtual int Padding() = 0;
		virtual Size3 OriginalDataSize() = 0;
		virtual Size3 CPUCacheSize() const= 0;
		Size3 CacheBlockDim()const { return cacheBlockDim; }
	};


	class CPUVolumeDataCache :public AbstrBlockedVolumeDataCPUCache
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
		LVDReader lvdReader;


		int blockCoordinateToBlockId(int xBlock, int yBlock, int zBlock)const
		{
			const auto size = lvdReader.SizeByBlock();
			const auto x = size.x, y = size.y, z = size.z;
			return zBlock * x*y + yBlock * x + xBlock;
		}
		void createCache();
		void initLRU();
	public:
		explicit CPUVolumeDataCache(const std::string& fileName,const Size3 & cacheBlockDim = {16,16,16});
		bool IsValid()const { return m_valid; }

		//std::size_t CPUCacheBlockCount() const { return totalCacheBlocks; }
		Size3 CPUCacheBlockSize()const
		{
			return Size3{ 1 << nLogBlockSize,1 << nLogBlockSize,1 << nLogBlockSize };
		}
		//ysl::Size3 CPUCacheDim()const { return cacheDim; }

		ysl::Size3 CPUCacheSize()const override { return CacheBlockDim()*(1<<nLogBlockSize); }
		int Padding()override;
		Size3 OriginalDataSize()override;
		Size3 BlockDim() override;
		Size3 BlockSize() override;
		const unsigned char * ReadBlockDataFromCPUCache(int xBlock, int yBlock, int zBlock)override{ return ReadBlockDataFromCPUCache(blockCoordinateToBlockId(xBlock, yBlock, zBlock)); }
		const unsigned char * ReadBlockDataFromCPUCache(int blockId)override;
		const unsigned char * ReadBlockDataFromCPUCache(const VirtualMemoryBlockIndex & index)override { return ReadBlockDataFromCPUCache(index.x, index.y, index.z); };
	};
}


#endif /*_LARGEVOLUMECACHE_H_*/
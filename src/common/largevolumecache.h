
#ifndef _LARGEVOLUMECACHE_H_
#define _LARGEVOLUMECACHE_H_
#include <map>
#include <list>
#include "lvdreader.h"
#include "geometry.h"
#include "common.h"

namespace ysl
{

	struct PageDirectoryEntryAbstractIndex
	{
		using internal_type = int;
		const internal_type x, y, z;
		PageDirectoryEntryAbstractIndex(internal_type x_ = -1,
			internal_type y_ = -1,
			internal_type z_ = -1)
			:x(x_), y(y_), z(z_) {}
	};

	struct PageTableEntryAbstractIndex
	{
		using internal_type = int;
		internal_type x, y, z;
		PageTableEntryAbstractIndex(internal_type x_ = -1,
			internal_type y_ = -1,
			internal_type z_ = -1) :
			x(x_), y(y_), z(z_) {}

	};

	struct PhysicalMemoryBlockIndex			// DataBlock start in 3d texture
	{
		using internal_type = int;
		const internal_type x, y, z;
	private:
		uint8_t unit = 0;
	public:
		PhysicalMemoryBlockIndex(internal_type x_ = -1,
			internal_type y_ = -1,
			internal_type z_ = -1) :
			x(x_), y(y_), z(z_),unit(0) {}
		PhysicalMemoryBlockIndex(internal_type x_,
			internal_type y_ ,
			internal_type z_ ,
			uint8_t unit) :
			x(x_), y(y_), z(z_),unit(unit) {}
		int GetPhysicalStorageUnit()const { return unit; }
		void SetPhysicalStorageUnit(uint8_t u) { unit = u; }
		Vec3i ToVec3i()const { return Vec3i{ x,y,z }; }
	};

	struct VirtualMemoryBlockIndex
	{
		VirtualMemoryBlockIndex() = default;
		VirtualMemoryBlockIndex(std::size_t linearId, int xb, int yb, int zb)
		{
			z = linearId / (xb*yb);
			y = (linearId - z * xb*yb) / xb;
			x = linearId - z * xb*yb - y * xb;
		}
		VirtualMemoryBlockIndex(int x, int y, int z):x(x),y(y),z(z){}
		Vec3i ToVec3i()const { return Vec3i{ x,y,z }; }

		using index_type = int;
		index_type x = -1, y=-1, z=-1;
	};

	class COMMON_EXPORT_IMPORT AbstrCPUBlockCache
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

	class COMMON_EXPORT_IMPORT AbstrBlockedVolumeDataCPUCache:public AbstrCPUBlockCache
	{
		const Size3 cacheBlockDim;
	public:
		AbstrBlockedVolumeDataCPUCache(const Size3 & blockDim) :cacheBlockDim(blockDim) {}
		virtual int Padding() = 0;
		virtual Size3 OriginalDataSize() = 0;
		virtual Size3 CPUCacheSize() const= 0;
		Size3 CacheBlockDim()const { return cacheBlockDim; }
	};

	class COMMON_EXPORT_IMPORT CPUVolumeDataCache :public AbstrBlockedVolumeDataCPUCache
	{
		static constexpr int nLogBlockSize = 6;		// 64
		static constexpr ysl::Size3 cacheBlockSize { 1 << nLogBlockSize,1 << nLogBlockSize,1 << nLogBlockSize };
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
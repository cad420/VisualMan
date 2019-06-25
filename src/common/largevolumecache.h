
#ifndef _LARGEVOLUMECACHE_H_
#define _LARGEVOLUMECACHE_H_
#include <map>
#include <list>
#include "lvdreader.h"
#include "geometry.h"
#include "common.h"
#include "genericcache.h"

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
			x(x_), y(y_), z(z_), unit(0) {}
		PhysicalMemoryBlockIndex(internal_type x_,
			internal_type y_,
			internal_type z_,
			uint8_t unit) :
			x(x_), y(y_), z(z_), unit(unit) {}
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
		VirtualMemoryBlockIndex(int x, int y, int z) :x(x), y(y), z(z) {}
		Vec3i ToVec3i()const { return Vec3i{ x,y,z }; }

		using index_type = int;
		index_type x = -1, y = -1, z = -1;
	};

	//struct PageID
	//{
	//	using PageIDType = int;
	//	PageIDType * idData;
	//	size_t count = 0;
	//	PageIDType GetPageID(size_t index)const { return idData[index]; }
	//	size_t GetPageIDCount()const { return count; }
	//};


	class CachePolicy
	{
	public:
		/**
		 * \brief Queries the page given by \a pageID if it exists in storage cache. Returns \a true if it exists or \a false if not
		 */
		virtual bool QueryPage(size_t pageID) = 0;
		/**
		 * \brief Updates the fault page given by \a pageID. Returns the actually storage ID of the page. If the page exists, the function does nothing. 
		 */
		virtual size_t UpdatePage(size_t pageID) = 0;
		/**
		 * \brief Queries and updates at the same time. It will always return a valid next-level page address. 
		 */
		virtual size_t QueryAndUpdate(size_t pageID) = 0;
	};

	class COMMON_EXPORT_IMPORT AbstraVirtualMemoryArch
	{
		std::shared_ptr<AbstraVirtualMemoryArch> nextLevel;
		std::unique_ptr<CachePolicy> cachePolicy;
	public:
		void SetNextLevelCache(std::shared_ptr<AbstraVirtualMemoryArch> cache) { nextLevel = cache; }
		void SetCachePolicy(std::unique_ptr<CachePolicy> policy) { cachePolicy = std::move(policy); }
		virtual const void* GetPage(int xBlock, int yBlock, int zBlock) = 0;
		virtual const void* GetPage(size_t pageID) = 0;
		virtual const void* GetPage(const VirtualMemoryBlockIndex& index) = 0;
		virtual size_t PageSize() = 0;
		virtual ~AbstraVirtualMemoryArch()=default;
	private:
		virtual void * GetPageStorage_Implement(size_t pageID) = 0;
	};

	class COMMON_EXPORT_IMPORT DiskPage:public AbstraVirtualMemoryArch
	{
	public:

	};

	class COMMON_EXPORT_IMPORT VirtualBlockedMemory:public AbstraVirtualMemoryArch
	{
		struct LRUListCell;
		using LRUList = std::list<LRUListCell>;
		using LRUHash = std::map<int, std::list<LRUListCell>::iterator>;

		struct LRUListCell
		{
			int blockCacheIndex;
			LRUHash::iterator hashIter;
			LRUListCell(int index, LRUHash::iterator itr) :blockCacheIndex{ index }, hashIter{ itr }{}
		};

		LRUList m_lruList;
		LRUHash	m_blockIdInCache;		// blockId---> (blockIndex,the position of blockIndex in list)
		std::unique_ptr<IBlock3DArrayAdapter> m_volumeCache;
		Size3 cacheDim;
		Size3 cacheSize;
		LVDReader lvdReader;

		int blockCoordinateToBlockId(int xBlock, int yBlock, int zBlock)const
		{
			const auto size = lvdReader.SizeByBlock();
			const auto x = size.x, y = size.y, z = size.z;
			return zBlock * x*y + yBlock * x + xBlock;
		}
		void Create();
		void InitLRU();

		void* GetPageStorage_Implement(size_t pageID) override;

	public:
		explicit VirtualBlockedMemory(const std::string& fileName);
		Size3 CPUCacheBlockSize()const{return Size3( 1 << lvdReader.BlockSizeInLog(),1 << lvdReader.BlockSizeInLog(),1 << lvdReader.BlockSizeInLog());}
		ysl::Size3 CPUCacheSize()const{ return CacheBlockDim()*(1 << lvdReader.BlockSizeInLog()); }
		int Padding();
		Size3 OriginalDataSize();
		Size3 CacheBlockDim()const { return cacheDim; }
		Size3 BlockDim();
		Size3 BlockSize();
		size_t PageSize() override { return BlockSize().Prod() * sizeof(char); }

		const void* GetPage(int xBlock, int yBlock, int zBlock) override { return GetPage(blockCoordinateToBlockId(xBlock, yBlock, zBlock)); }

		const void* GetPage(size_t pageID) override;

		const void* GetPage(const VirtualMemoryBlockIndex& index) override { return GetPage(index.x, index.y, index.z); };
	};
}


#endif /*_LARGEVOLUMECACHE_H_*/
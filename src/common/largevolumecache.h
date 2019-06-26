
#ifndef _LARGEVOLUMECACHE_H_
#define _LARGEVOLUMECACHE_H_
#include <map>
#include <list>
#include "lvdreader.h"
#include "geometry.h"
#include "common.h"
#include "genericcache.h"
#include "virtualmemorymanager.h"

namespace ysl
{

	class LRUCachePolicy:public AbstrCachePolicy
	{
		struct LRUListCell;
		using LRUList = std::list<LRUListCell>;
		using LRUHash = std::map<int, std::list<LRUListCell>::iterator>;
		struct LRUListCell
		{
			size_t storageID;
			LRUHash::iterator hashIter;
			LRUListCell(size_t index, LRUHash::iterator itr) :storageID{ index }, hashIter{ itr }{}
		};
		LRUList m_lruList;
		LRUHash	m_blockIdInCache;		// blockId---> (blockIndex,the position of blockIndex in list)
	public:
		LRUCachePolicy() = default;
		bool QueryPage(size_t pageID) override;
		void UpdatePage(size_t pageID) override;
		size_t QueryAndUpdate(size_t pageID) override;
	protected:
		void InitEvent(AbstrMemoryCache* cache) override;
	};


	class COMMON_EXPORT_IMPORT DiskPageAdapter:public AbstrMemoryCache
	{
		LVDReader lvdReader;
	public:
		DiskPageAdapter(const std::string & fileName):lvdReader(fileName){}
		const void * GetPage(int xBlock, int yBlock, int zBlock) override;
		const void * GetPage(size_t pageID) override;
		const void * GetPage(const VirtualMemoryBlockIndex& index) override;
		size_t GetPageSize() override { return lvdReader.BlockSize(); }
		size_t GetPhysicalPageCount() override { return lvdReader.BlockCount(); }
		size_t GetVirtualPageCount() override { return lvdReader.BlockCount(); }
	private:
		void * GetPageStorage_Implement(size_t pageID) override { return nullptr; }
	};

	class COMMON_EXPORT_IMPORT MemoryPageAdapter:public AbstrMemoryCache
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
	public:
		explicit MemoryPageAdapter(const std::string& fileName);
		Size3 CPUCacheBlockSize()const{return Size3( 1 << lvdReader.BlockSizeInLog(),1 << lvdReader.BlockSizeInLog(),1 << lvdReader.BlockSizeInLog());}
		ysl::Size3 CPUCacheSize()const{ return CacheBlockDim()*(1 << lvdReader.BlockSizeInLog()); }
		int Padding();
		Size3 OriginalDataSize();
		Size3 CacheBlockDim()const { return cacheDim; }
		Size3 BlockDim();
		Size3 BlockSize();

		size_t GetPhysicalPageCount() override { return CacheBlockDim().Prod(); }
		size_t GetVirtualPageCount() override { return BlockDim().Prod(); }
		size_t GetPageSize() override { return BlockSize().Prod() * sizeof(char); }
		const void* GetPage(int xBlock, int yBlock, int zBlock) override { return GetPage(blockCoordinateToBlockId(xBlock, yBlock, zBlock)); }
		const void* GetPage(size_t pageID) override;
		const void* GetPage(const VirtualMemoryBlockIndex& index) override { return GetPage(index.x, index.y, index.z); };
	protected:
		void * GetPageStorage_Implement(size_t pageID) override;
	};
}


#endif /*_LARGEVOLUMECACHE_H_*/
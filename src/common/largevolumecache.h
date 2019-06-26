
#ifndef _LARGEVOLUMECACHE_H_
#define _LARGEVOLUMECACHE_H_
#include <list>
#include "lvdreader.h"
#include "geometry.h"
#include "common.h"
#include "genericcache.h"
#include "virtualmemorymanager.h"

namespace ysl
{


	class COMMON_EXPORT_IMPORT Disk3DPageAdapter:public AbstrMemoryCache
	{
		LVDReader lvdReader;
	public:
		Disk3DPageAdapter(const std::string & fileName):lvdReader(fileName){}
		const void * GetPage(size_t pageID) override { return lvdReader.ReadBlock(pageID); }
		size_t GetPageSize() override { return lvdReader.BlockSize(); }
		size_t GetPhysicalPageCount() override { return lvdReader.BlockCount(); }
		size_t GetVirtualPageCount() override { return lvdReader.BlockCount(); }

		int GetPadding()const { return lvdReader.GetBlockPadding(); }
		Size3 GetDataSizeWithoutPadding()const { return lvdReader.OriginalDataSize(); }
		Size3 Get3DPageSize()const
		{
			const std::size_t len = lvdReader.BlockSize();
			return Size3{ len,len,len };	
		}
		int Get3DPageSizeInLog()const { return lvdReader.BlockSizeInLog(); }
		Size3 Get3DPageCount()const { return lvdReader.SizeByBlock(); }
	private:
		void * GetPageStorage_Implement(size_t pageID) override { return nullptr; }
	};

	class COMMON_EXPORT_IMPORT MemoryPageAdapter:public AbstrMemoryCache
	{

		Size3 cacheDim;
		std::unique_ptr<IBlock3DArrayAdapter> m_volumeCache;
		std::shared_ptr<Disk3DPageAdapter> adapter;
		[[deprecated]] int blockCoordinateToBlockId(int xBlock, int yBlock, int zBlock) const;
		void Create();
	public:
		explicit MemoryPageAdapter(const std::string& fileName);

		void SetDiskFileCache(std::shared_ptr<Disk3DPageAdapter> diskCache);

		Size3 CPUCacheBlockSize() const;

		ysl::Size3 CPUCacheSize() const;

		[[deprecated]] int Padding();
		[[deprecated]] Size3 OriginalDataSize();
		[[deprecated]] Size3 BlockDim();
		[[deprecated]] Size3 BlockSize();

		Size3 CacheBlockDim()const { return cacheDim; }
		size_t GetPhysicalPageCount() override { return CacheBlockDim().Prod(); }
		size_t GetVirtualPageCount() override { return BlockDim().Prod(); }
		size_t GetPageSize() override { return BlockSize().Prod() * sizeof(char); }
		const void* GetPage(int xBlock, int yBlock, int zBlock)  { return AbstrMemoryCache::GetPage(blockCoordinateToBlockId(xBlock, yBlock, zBlock)); }
		const void* GetPage(const VirtualMemoryBlockIndex& index) { return GetPage(index.x, index.y, index.z); };

	protected:
		[[deprecated]] int GetLog()const;
		void * GetPageStorage_Implement(size_t pageID) override;
	};
}


#endif /*_LARGEVOLUMECACHE_H_*/
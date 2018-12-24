
#ifndef _VIRTUALVOLUMEHIERACHY_H_
#define _VIRTUALVOLUMEHIERACHY_H_

#include "../memory/lineararray.h"

#include "volume.h"

#include <list>



struct PageDirectoryEntryAbstractIndex
{
	using internal_type = int;
	const internal_type x, y, z;
	PageDirectoryEntryAbstractIndex(internal_type x_ =-1,
		internal_type y_ = -1,
		internal_type z_ = -1)
		:x(x_), y(y_), z(z_) {}
};

struct PageTableEntryAbstractIndex
{
	using internal_type = int;
	const internal_type x, y, z;
	PageTableEntryAbstractIndex(internal_type x_ = -1,
		internal_type y_ = -1,
		internal_type z_ = -1) :
		x(x_), y(y_), z(z_) {}
};

struct CacheBlockAbstractIndex			// DataBlock start in 3d texture
{
	using internal_type = int;
	const internal_type x, y, z;
	CacheBlockAbstractIndex(internal_type x_ = -1,
		internal_type y_ = -1,
		internal_type z_ = -1) :
		x(x_), y(y_), z(z_) {}
};



enum EntryFlag { Empty = 0, Unmapped = 2, Mapped = 1 };

template<
int xPageTableEntry,
int yPageTableEntry, 
int zPageTableEntry>
class VolumeVirtualMemoryHierachyGenerator:public LargeVolumeCache
{
public:
	

	struct PageDirEntry
	{
		int x, y, z, w;
	};
	struct PageTableEntry
	{
		int x, y, z, w;
	};

	std::unique_ptr<ysl::Linear3DArray<PageDirEntry>> PageDir;
	std::unique_ptr<ysl::Linear3DArray<PageTableEntry>> PageTable;

	std::list<std::pair<PageTableEntryAbstractIndex, CacheBlockAbstractIndex>> m_lruList;
	//unsigned int m_pageDirTextureId;
	//unsigned int m_pageTableTextureId;
	//unsigned int m_cacheTextureId;

	void initPageDir() 
	{
		for (auto z = 0; z < PageDir->Depth(); z++)
			for (auto y = 0; y < PageDir->Height(); y++)
				for (auto x = 0; x < PageDir->Width(); x++)
				{
					PageDirEntry dirEntry;
					dirEntry.x = x * xPageTableEntry;
					dirEntry.y = y * yPageTableEntry;
					dirEntry.z = z * zPageTableEntry;
					dirEntry.w = Mapped;
					(*PageDir)(x, y, z) = dirEntry;

				}
	}

	void initPageTable()
	{
		// Only initialization flag filed, the table entry is determined by cache miss at run time using lazy evaluation policy.
		auto d = PageTable->Data();
		for (auto z = 0; z < PageTable->Depth(); z++)
			for (auto y = 0; y < PageTable->Height(); y++)
				for (auto x = 0; x < PageTable->Width(); x++)
				{
					PageTableEntry entry;
					entry.x = -1;
					entry.y = -1;
					entry.z = -1;
					entry.w = Unmapped;
					(*PageTable)(x, y, z) = entry;
				}
	}



	void initLRUList()
	{
		const auto tot = cacheBlockCount();
		const auto w = xCacheBlockCount(), h = yCacheBlockCount(), d = zCacheBlockCount();
		for (auto z = 0; z < d; z++)
			for (auto y = 0; y < h; y++)
				for (auto x = 0; x < w; x++)
					m_lruList.push_back(std::make_pair({-1,-1,-1}, {x,y,z}));
	}

public:
	using size_type = std::size_t;
	VolumeVirtualMemoryHierachyGenerator(const std::string fileName):LargeVolumeCache(fileName)
		
	{
		using ysl::RoundUpDivide;
		using ysl::Linear3DArray;

		PageTable.reset(new Linear3DArray<PageTableEntry>(xBlockCount(), yBlockCount(), zBlockCount(), nullptr));
		PageDir.reset(new Linear3DArray<PageDirEntry>(RoundUpDivide(PageTable->Width(), xPageTableEntry), RoundUpDivide(PageTable->Height(), yPageTableEntry), RoundUpDivide(PageTable->Depth(), zPageTableEntry), nullptr));
		initPageDir();
		initPageTable();

	}

	void bindPageDirectoryTexture(unsigned int textureId)
	{
		
	}

	void bindPageTableTexture(unsigned int textureId)
	{
		
	}

	void bindBlockCacheTexture(unsigned int textureId)
	{
		
	}



	void updateCacheMiss(const std::vector<GlobalBlockAbstractIndex> & hits)
	{
		for(const auto & i:hits)
		{
			(*PageTable)(i.x, i.y, i.z).w = Mapped;
			auto & last = m_lruList.back();
			(*PageTable)(last.first.x, last.first.y, last.first.z).w = Unmapped;
			m_lruList.splice(m_lruList.begin(), m_lruList, --m_lruList.end());		// move from tail to head
			auto d = blockData(i);

		}
	}

};
#endif
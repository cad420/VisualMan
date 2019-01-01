
#ifndef _VIRTUALVOLUMEHIERACHY_H_
#define _VIRTUALVOLUMEHIERACHY_H_

#include "../memory/lineararray.h"

#include "volume.h"

#include <list>
#include "texture.h"


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
	internal_type x, y, z;
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
class VolumeVirtualMemoryHierachy:public LargeVolumeCache
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

	void initPageDir() 
	{
		for (auto z = 0; z < PageDir->Size().z; z++)
			for (auto y = 0; y < PageDir->Size().y; y++)
				for (auto x = 0; x < PageDir->Size().x; x++)
				{
					PageDirEntry dirEntry;
					dirEntry.x = x * xPageTableEntry;
					dirEntry.y = y * yPageTableEntry;
					dirEntry.z = z * zPageTableEntry;
					dirEntry.w = Mapped;
					(*PageDir)(x, y, z) = dirEntry;
				}
	}

	/**
	 * \brief Initializes the page table with all entries are unmapped
	 */

	void initPageTable()
	{
		// Only initialization flag filed, the table entry is determined by cache miss at run time using lazy evaluation policy.
		//auto d = PageTable->Data();
		for (auto z = 0; z < PageTable->Size().z; z++)
			for (auto y = 0; y < PageTable->Size().y; y++)
				for (auto x = 0; x < PageTable->Size().x; x++)
				{
					PageTableEntry entry;
					entry.x = -1;
					entry.y = -1;
					entry.z = -1;
					entry.w = Unmapped;
					(*PageTable)(x, y, z) = entry;
				}
	}

	/**
	 * \brief  Initializes the page table with all entries are mapped
	 */
	void initPageTable(int xBlockSize,int yBlockSize,int zBlockSize)
	{
		//auto d = PageTable->Data();
		for (auto z = 0; z < PageTable->Size().z; z++)
			for (auto y = 0; y < PageTable->Size().y; y++)
				for (auto x = 0; x < PageTable->Size().x; x++)
				{
					PageTableEntry entry;
					entry.x = x*xBlockSize;
					entry.y = y*yBlockSize;
					entry.z = z*zBlockSize;
					entry.w = Unmapped;
					(*PageTable)(x, y, z) = entry;
				}
	}


	void initLRUList()
	{
		//const auto tot = cacheBlockCount();
		const auto size = BlockSize();
		//const auto w = xCacheBlockCount(), h = yCacheBlockCount(), d = zCacheBlockCount();
		const auto dim = CacheDim();
		for (auto z = 0; z < dim.z; z++)
			for (auto y = 0; y < dim.x; y++)
				for (auto x = 0; x < dim.y; x++) 
				{
					m_lruList.push_back(std::make_pair(PageTableEntryAbstractIndex(-1,-1,-1), CacheBlockAbstractIndex(x*size,y*size,z*size)));
				}
	}

public:
	using size_type = std::size_t;
	VolumeVirtualMemoryHierachy(const std::string fileName):LargeVolumeCache(fileName)
	{
		using ysl::RoundUpDivide;
		using ysl::Linear3DArray;

		auto sizeByBlock = SizeByBlock();

		PageTable.reset(new Linear3DArray<PageTableEntry>(
			sizeByBlock.x,
			sizeByBlock.y,
			sizeByBlock.z,
			nullptr ));
		PageDir.reset(new Linear3DArray<PageDirEntry> (
			RoundUpDivide(PageTable->Size().x, xPageTableEntry),
			RoundUpDivide(PageTable->Size().y, yPageTableEntry),
			RoundUpDivide(PageTable->Size().z, zPageTableEntry),
			nullptr)
		);

		initPageDir();
		initPageTable(BlockSize(), BlockSize(), BlockSize());
		initLRUList();
	}

	void updateCacheMiss(const std::vector<GlobalBlockAbstractIndex> & hits)
	{
		for(const auto & i:hits)
		{
			(*PageTable)(i.x, i.y, i.z).w = Mapped;
			auto & last = m_lruList.back();
			(*PageTable)(last.first.x, last.first.y, last.first.z).w = Unmapped;
			m_lruList.splice(m_lruList.begin(), m_lruList, --m_lruList.end());		// move from tail to head
			auto d = ReadBlockDataFromCache(i);

		}
	}

};


class BlockCacheTexture:public OpenGLTexture
{
	ysl::Size3 cacheSize;
public:
	BlockCacheTexture(const ysl::Size3 & size):OpenGLTexture(Texture3D,
		OpenGLTexture::Linear, 
		OpenGLTexture::Linear,
		OpenGLTexture::ClampToEdge, 
		OpenGLTexture::ClampToEdge, 
		OpenGLTexture::ClampToEdge),cacheSize(size)
	{

	}

	void Resize(const ysl::Size3 & size);

};

class PageTableTexture:public OpenGLTexture
{
	ysl::Size3 pageTableSize;
	const static InternalFormat internalFormat = InternalFormat::RGBA32UI;
	const static ExternalDataFormat extFormat = RGBAInteger;
	const static ExternalDataType extType = UInt32;
public:
	PageTableTexture(const ysl::Size3 & size) :OpenGLTexture(Texture3D,
		OpenGLTexture::Linear,
		OpenGLTexture::Linear,
		OpenGLTexture::ClampToEdge,
		OpenGLTexture::ClampToEdge,
		OpenGLTexture::ClampToEdge),pageTableSize(size)
	{

	}
	void LoadPageTable(const void * data);
	//void UpdateDataPageTableEntry(const PageTableEntryAbstractIndex & index);
};

#endif
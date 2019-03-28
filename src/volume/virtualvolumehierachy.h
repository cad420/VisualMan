
#ifndef _VIRTUALVOLUMEHIERACHY_H_
#define _VIRTUALVOLUMEHIERACHY_H_

#include "../memory/lineararray.h"

#include "volume.h"
#include "largevolumecache.h"

#include <list>
#include "texture.h"


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
		PhysicalMemoryBlockIndex(internal_type x_ = -1,
			internal_type y_ = -1,
			internal_type z_ = -1) :
			x(x_), y(y_), z(z_) {}
	};

	enum EntryFlag { Empty = 0, Unmapped = 2, Mapped = 1 };


	class VirtualMemoryManager
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

	private:
		//Linear3DArray<PageDirEntry> PageDir;
		Linear3DArray<PageTableEntry> pageTable;
		std::list<std::pair<PageTableEntryAbstractIndex, PhysicalMemoryBlockIndex>> g_lruList;

		const Size3 physicalMemoryBlock;
		const Size3 virtualMemoryBlock;
		const Size3 blockSize;
		CPUVolumeDataCache * const cacheData;
		void initPageDir();
		void initPageTable(const Size3& blockDim);

		 //	const auto blockSize = BlockSize();

		 //	PageTable = Linear3DArray<PageTableEntry>(blockDim, nullptr);

		 //	for (auto z = 0; z < PageTable.Size().z; z++)
		 //		for (auto y = 0; y < PageTable.Size().y; y++)
		 //			for (auto x = 0; x < PageTable.Size().x; x++)
		 //			{
		 //				PageTableEntry entry;
		 //				entry.x = x * blockSize;
		 //				entry.y = y * blockSize;
		 //				entry.z = z * blockSize;
		 //				entry.w = Unmapped;
		 //				(PageTable)(x, y, z) = entry;
		 //			}
		 //}

		void initLRUList(const Size3& physicalMemoryBlock, const Size3& page3DSize);

	public:
		using size_type = std::size_t;
		VirtualMemoryManager(const Size3& physicalMemoryBlock, CPUVolumeDataCache * virtualData): physicalMemoryBlock(physicalMemoryBlock),
			cacheData(virtualData),
			virtualMemoryBlock(virtualData->SizeByBlock()),
			blockSize({ (std::size_t)virtualData->BlockSize(), (std::size_t)virtualData->BlockSize(),  (std::size_t)virtualData->BlockSize() })
		{
			initPageTable(virtualMemoryBlock);
			initLRUList(physicalMemoryBlock, blockSize);
		}

		std::vector<PhysicalMemoryBlockIndex> UpdatePageTable(const std::vector<VirtualMemoryBlockIndex>& missedBlockIndices);
		Linear3DArray<PageTableEntry>& PageTable(int level);
		Size3 PageTableSize();
		Size3 BlockSize()const;

		CPUVolumeDataCache * VirtualData() { return cacheData; }
	};



	class BlockCacheTexture :public OpenGLTexture
	{
		ysl::Size3 cacheSize;
	public:
		BlockCacheTexture(const ysl::Size3 & size) :OpenGLTexture(Texture3D,
			OpenGLTexture::Linear,
			OpenGLTexture::Linear,
			OpenGLTexture::ClampToEdge,
			OpenGLTexture::ClampToEdge,
			OpenGLTexture::ClampToEdge), cacheSize(size)
		{

		}

		void Resize(const ysl::Size3 & size);
	};

	class PageTableTexture :public OpenGLTexture
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
			OpenGLTexture::ClampToEdge), pageTableSize(size)
		{

		}
		void LoadPageTable(const void * data);
		//void UpdateDataPageTableEntry(const PageTableEntryAbstractIndex & index);
	};

}

#endif

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

	template<
		int xPageTableEntry,
		int yPageTableEntry,
		int zPageTableEntry>
		class VolumeVirtualMemoryHierarchy :public LargeVolumeCache
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
		Linear3DArray<PageDirEntry> PageDir;
		Linear3DArray<PageTableEntry> PageTable;
		/////
		std::list<std::pair<PageTableEntryAbstractIndex, PhysicalMemoryBlockIndex>> g_lruList;

		const Size3 physicalMemoryBlock;
		const Size3 virtualMemoryBlock;
			/**
		 * \brief 
		 * \param missedBlockIndices 
		 * \return 
		 */



		/////

	private:

		std::unique_ptr<LargeVolumeCache> volumeCache;

		void initPageDir()
		{

			PageDir = Linear3DArray<PageDirEntry>(
				RoundUpDivide(PageTable.Size().x, xPageTableEntry),
				RoundUpDivide(PageTable.Size().y, yPageTableEntry),
				RoundUpDivide(PageTable.Size().z, zPageTableEntry),
				nullptr
				);

			for (auto z = 0; z < PageDir.Size().z; z++)
				for (auto y = 0; y < PageDir.Size().y; y++)
					for (auto x = 0; x < PageDir.Size().x; x++)
					{
						PageDirEntry dirEntry;
						dirEntry.x = x * xPageTableEntry;
						dirEntry.y = y * yPageTableEntry;
						dirEntry.z = z * zPageTableEntry;
						dirEntry.w = Mapped;
						(PageDir)(x, y, z) = dirEntry;
					}
		}

		/**
		 * \brief Initializes the page table with all entries are unmapped
		 */

		void  initPageTable(const Size3& blockDim)
		{
			// Only initialization flag filed, the table entry is determined by cache miss at run time using lazy evaluation policy.

			PageTable = Linear3DArray<PageTableEntry>(blockDim, nullptr);
			for (auto z = 0; z < PageTable.Size().z; z++)
				for (auto y = 0; y < PageTable.Size().y; y++)
					for (auto x = 0; x < PageTable.Size().x; x++)
					{
						PageTableEntry entry;
						//entry.x = x * blockSize;
						//entry.y = y * blockSize;
						//entry.z = z * blockSize;
						entry.x = -1;
						entry.y = -1;
						entry.z = -1;
						entry.w = Unmapped;
						(PageTable)(x, y, z) = entry;
					}
		}

		/**
		 * \brief  Initializes the page table with all entries are mapped
		 */

		 //void initPageTable(const Size3& blockDim)
		 //{


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


		void initLRUList()
		{
			//const auto tot = cacheBlockCount();
			//const auto size = BlockSize();
			////const auto w = xCacheBlockCount(), h = yCacheBlockCount(), d = zCacheBlockCount();
			//const auto dim = CacheDim();
			//for (auto z = 0; z < dim.x; z++)
			//	for (auto y = 0; y < dim.y; y++)
			//		for (auto x = 0; x < dim.z; x++) 
			//		{
			//			m_lruList.push_back(std::make_pair(PageTableEntryAbstractIndex(-1,-1,-1), CacheBlockAbstractIndex(x*size,y*size,z*size)));
			//		}
		}

	public:
		using size_type = std::size_t;
		VolumeVirtualMemoryHierarchy(const std::string fileName) :LargeVolumeCache(fileName)
		{

			initPageTable(SizeByBlock());

			initPageDir();

			initLRUList();
		}

		VolumeVirtualMemoryHierarchy(const Size3 & physicalMemoryBlock,const Size3 & virtualMemoryBlock):LargeVolumeCache(""),
		virtualMemoryBlock(virtualMemoryBlock),
		physicalMemoryBlock(physicalMemoryBlock)
		{
			
		}

		

		std::vector<PhysicalMemoryBlockIndex> UpdatePageTable(const std::vector<VirtualMemoryBlockIndex> & missedBlockIndices);

		/**
	 * \brief Returns \a level th page table data
	 * \return
	 */
		Linear3DArray<PageTableEntry> PageTableData(int level)const;

		//VolumeVirtualMemoryHierarchy(const Size3 & physicalMemoryBlockDim)
		//{
		//	
		//}



		//void updateCacheMiss(const std::vector<GlobalBlockAbstractIndex> & hits)
		//{
		//	for(const auto & i:hits)
		//	{
		//		(*PageTable)(i.x, i.y, i.z).w = Mapped;
		//		auto & last = m_lruList.back();
		//		(*PageTable)(last.first.x, last.first.y, last.first.z).w = Unmapped;
		//		m_lruList.splice(m_lruList.begin(), m_lruList, --m_lruList.end());		// move from tail to head
		//		auto d = ReadBlockDataFromCache(i);
		//	}
		//}

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
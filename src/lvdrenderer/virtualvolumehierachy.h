
#ifndef _VIRTUALVOLUMEHIERACHY_H_
#define _VIRTUALVOLUMEHIERACHY_H_

#include "lineararray.h"
#include <largevolumecache.h>
#include <list>
#include "gpupagetable.h"
#include "gpuvolumedatacache.h"

namespace ysl
{


	enum EntryFlag { Empty = 0, Unmapped = 2, Mapped = 1 };


	class CPUVolumeDataCache;
	class GPUVolumeDataCache;
	class GPUPageTableDataCache;

	class PageTableManager
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
		//const Size3 physicalMemoryBlock;
		//const Size3 virtualMemoryBlock;
		//const Size3 blockSize;
		//CPUVolumeDataCache * const cacheData;

		const std::shared_ptr<CPUVolumeDataCache> cacheData;
		const std::shared_ptr<GPUVolumeDataCache> gpuCacheData;

		std::shared_ptr<GPUPageTableDataCache> texPageTable;

		void InitGPUPageTable();
		void InitCPUPageTable(const Size3& blockDim);
		void InitLRUList(const Size3& physicalMemoryBlock, const Size3& page3DSize);
	public:
		using size_type = std::size_t;
		//PageTableManager(const Size3 & physicalMemoryBlock, std::shared_ptr<CPUVolumeDataCache> virtualData): physicalMemoryBlock(physicalMemoryBlock),
		//virtualMemoryBlock(virtualData->BlockDim()),
		//blockSize(virtualData->BlockSize()),
		//cacheData(virtualData)
		//{

		//	InitCPUPageTable(virtualMemoryBlock);
		//	InitLRUList(physicalMemoryBlock, blockSize);
		//	InitGPUPageTable();
		//}

		PageTableManager(std::shared_ptr<GPUVolumeDataCache>  physicalData, std::shared_ptr<CPUVolumeDataCache> virtualData):
			cacheData(std::move(virtualData)),
			gpuCacheData(std::move(physicalData))
		{

			InitCPUPageTable(cacheData->BlockDim());
			InitLRUList(gpuCacheData->BlockDim(), gpuCacheData->BlockSize());
			InitGPUPageTable();
		}

		std::vector<PhysicalMemoryBlockIndex> UpdatePageTable(const std::vector<VirtualMemoryBlockIndex>& missedBlockIndices);
		Linear3DArray<PageTableEntry> & PageTable(int level);
		Size3 PageTableSize();
		Size3 BlockSize()const;
		void BindTextureToImage(int index);
		void UpdatePageTable();
		std::shared_ptr<CPUVolumeDataCache> VirtualData();

	};
}

#endif
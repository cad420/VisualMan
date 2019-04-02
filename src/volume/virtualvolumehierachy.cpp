
#include "virtualvolumehierachy.h"
#include "../application/largevolumeraycasterapplication.h"

namespace ysl
{
	void VirtualMemoryManager::initPageDir()
	{
		//PageDir = Linear3DArray<PageDirEntry>(
		//	RoundUpDivide(PageTable.Size().x, xPageTableEntry),
		//	RoundUpDivide(PageTable.Size().y, yPageTableEntry),
		//	RoundUpDivide(PageTable.Size().z, zPageTableEntry),
		//	nullptr
		//	);

		//for (auto z = 0; z < PageDir.Size().z; z++)
		//	for (auto y = 0; y < PageDir.Size().y; y++)
		//		for (auto x = 0; x < PageDir.Size().x; x++)
		//		{
		//			PageDirEntry dirEntry;
		//			dirEntry.x = x * xPageTableEntry;
		//			dirEntry.y = y * yPageTableEntry;
		//			dirEntry.z = z * zPageTableEntry;
		//			dirEntry.w = Mapped;
		//			(PageDir)(x, y, z) = dirEntry;
		//		}
	}

	void VirtualMemoryManager::initPageTable(const Size3& blockDim)
	{
		// Only initialization flag filed, the table entry is determined by cache miss at run time using lazy evaluation policy.
		pageTable = Linear3DArray<PageTableEntry>(blockDim, nullptr);
		for (auto z = 0; z < pageTable.Size().z; z++)
			for (auto y = 0; y < pageTable.Size().y; y++)
				for (auto x = 0; x < pageTable.Size().x; x++)
				{
					PageTableEntry entry;
					entry.x = -1;
					entry.y = -1;
					entry.z = -1;
					entry.w = Unmapped;
					(pageTable)(x, y, z) = entry;
				}
	}

	void VirtualMemoryManager::initLRUList(const Size3& physicalMemoryBlock, const Size3& page3DSize)
	{
		for (auto z = 0; z < physicalMemoryBlock.z; z++)
			for (auto y = 0; y < physicalMemoryBlock.y; y++)
				for (auto x = 0; x < physicalMemoryBlock.x; x++)
				{
					g_lruList.emplace_back(
						PageTableEntryAbstractIndex(-1, -1, -1),
						PhysicalMemoryBlockIndex( x * page3DSize.x, y * page3DSize.y, z * page3DSize.z )
					);
				}
	}
	//VirtualMemoryManager::VirtualMemoryManager(const Size3& physicalMemoryBlock,
	//										   const Size3& virtualMemoryBlock,
	//                                           const Size3& blockSize):
	//	physicalMemoryBlock(physicalMemoryBlock),
	//	virtualMemoryBlock(virtualMemoryBlock),
	//	blockSize(blockSize)
	//{
	//	initPageTable(virtualMemoryBlock);
	//	initLRUList(physicalMemoryBlock, blockSize);
	//}
	std::vector<PhysicalMemoryBlockIndex> VirtualMemoryManager::UpdatePageTable(
		const std::vector<VirtualMemoryBlockIndex>& missedBlockIndices)
	{
		const auto missedBlocks = missedBlockIndices.size();

		std::vector<PhysicalMemoryBlockIndex> physicalIndices;
		// Update LRU List 
		for (int i = 0; i < missedBlocks; i++)
		{
			const auto& index = missedBlockIndices[i];
			auto& pageTableEntry = pageTable(index.x, index.y, index.z);
			auto& last = g_lruList.back();
			pageTableEntry.w = EntryFlag::Mapped; // Map the flag of page table entry
			// last.second is the cache block index

			const auto xInCache = last.second.x;
			const auto yInCache = last.second.y;
			const auto zInCache = last.second.z;

			pageTableEntry.x = xInCache; // fill the page table entry
			pageTableEntry.y = yInCache;
			pageTableEntry.z = zInCache;

			if (last.first.x != -1)
			{
				pageTable(last.first.x, last.first.y, last.first.z).w = EntryFlag::Unmapped;
			}

			last.first.x = index.x;
			last.first.y = index.y;
			last.first.z = index.z;
			g_lruList.splice(g_lruList.begin(), g_lruList, --g_lruList.end()); // move from tail to head, LRU policy

			physicalIndices.push_back({xInCache, yInCache, zInCache});

			//std::cout << last.first.x << " " << last.first.y << " " << last.first.z << " " << xInCache << " " << yInCache << " " << zInCache << std::endl;
		}

		return physicalIndices;
	}

	Linear3DArray<VirtualMemoryManager::PageTableEntry>& VirtualMemoryManager::PageTable(int level)
	{
		assert(level == 0);
		return pageTable; 
	}

	Size3 VirtualMemoryManager::PageTableSize()
	{
		return pageTable.Size();
	}

	Size3 VirtualMemoryManager::BlockSize() const
	{
		return blockSize;
	}

	//const Linear3DArray<PageTableEntry> &PageTableManager::PageTable(int level) const
	//{
	//	assert(level == 0);
	//	return pageTable; 
	//}

	//Size3 PageTableManager::PageTableSize() const
	//{
	//	return pageTable.Size();
	//}
}


#include "largevolumecache.h"
#include <iostream>
#include <cassert>
#include <combaseapi.h>
#include "error.h"


#define SHOW_LIST_STATE																										\
		std::cout << "LRU State:" << std::endl;																				\
		for (auto it = m_lruList.begin(); it != m_lruList.end(); ++it)														\
		{																													\
			if (it->hashIter != m_blockIdInCache.end())																		\
			std::cout << "[BlockID:" << it->hashIter->first << " -> CacheID:" << it->blockCacheIndex << "]--->";			\
		}																													\
		std::cout << std::endl;																								\


namespace ysl
{
	bool LRUCachePolicy::QueryPage(size_t pageID)
	{
		return m_blockIdInCache.find(pageID) == m_blockIdInCache.end() ? false : true;
	}

	void LRUCachePolicy::UpdatePage(size_t pageID)
	{
		const auto it = m_blockIdInCache.find(pageID);
		if (it == m_blockIdInCache.end())
		{
			m_lruList.splice(m_lruList.begin(), m_lruList, it->second);			// move the node that it->second points to the head.
		}
	}

	size_t LRUCachePolicy::QueryAndUpdate(size_t pageID)
	{
		const auto it = m_blockIdInCache.find(pageID);
		if (it == m_blockIdInCache.end())
		{
			// replace the last block in cache
			auto & lastCell = m_lruList.back();
			m_lruList.splice(m_lruList.begin(), m_lruList, --m_lruList.end());		// move last to head

			const auto newItr = m_blockIdInCache.insert(std::make_pair(pageID, m_lruList.begin()));
			if (lastCell.hashIter != m_blockIdInCache.end())
			{
				m_blockIdInCache.erase(lastCell.hashIter); // Unmapped old
			}
			lastCell.hashIter = newItr.first;		// Mapping new 
			return lastCell.storageID;
		}
		else
		{
			m_lruList.splice(m_lruList.begin(), m_lruList, it->second);			// move the node that it->second points to the head.
			return it->second->storageID;
		}
	}


	void LRUCachePolicy::InitEvent(AbstrMemoryCache* cache)
	{
		assert(cache);
		for (auto i = std::size_t(0); i < cache->GetPhysicalPageCount(); i++)
			m_lruList.push_front(LRUListCell(i, m_blockIdInCache.end()));
	}

	const void* DiskPageAdapter::GetPage(int xBlock, int yBlock, int zBlock)
	{
		assert(false);
		return nullptr;
	}

	const void* DiskPageAdapter::GetPage(size_t pageID)
	{
		return lvdReader.ReadBlock(pageID);
	}

	const void* DiskPageAdapter::GetPage(const VirtualMemoryBlockIndex& index)
	{
		return nullptr;
	}

	void MemoryPageAdapter::Create()
	{
		switch (lvdReader.BlockSizeInLog())
		{
		case 6:m_volumeCache = std::make_unique<Int8Block64Cache>(cacheSize.x, cacheSize.y, cacheSize.z, nullptr); break;
		case 7:m_volumeCache = std::make_unique<Int8Block128Cache>(cacheSize.x, cacheSize.y, cacheSize.z, nullptr); break;
		case 8:m_volumeCache = std::make_unique<Int8Block256Cache>(cacheSize.x, cacheSize.y, cacheSize.z, nullptr); break;
		case 9:m_volumeCache = std::make_unique<Int8Block512Cache>(cacheSize.x, cacheSize.y, cacheSize.z, nullptr); break;
		case 10:m_volumeCache = std::make_unique<Int8Block1024Cache>(cacheSize.x, cacheSize.y, cacheSize.z, nullptr); break;
		default:Warning("Unsupported Cache block Size\n"); break;
		}
		if (!m_volumeCache)
		{
			std::cerr << "Can not allocate memory for cache\n";
			exit(0);
		}
	}

	void MemoryPageAdapter::InitLRU()
	{
		for (auto i = std::size_t(0); i < cacheDim.Prod(); i++)
			m_lruList.push_front(LRUListCell(i, m_blockIdInCache.end()));
	}

	void* MemoryPageAdapter::GetPageStorage_Implement(size_t pageID)
	{
		return nullptr;
	}

	MemoryPageAdapter::MemoryPageAdapter(const std::string& fileName) :
		lvdReader(fileName)
	{
		//cacheDim = cacheBlockDim;
		cacheDim = Size3(16, 16, 16);
		cacheSize = cacheDim * Size3(1 << lvdReader.BlockSizeInLog(), 1 << lvdReader.BlockSizeInLog(), 1 << lvdReader.BlockSizeInLog());
		Create();
		InitLRU();
		//sizeByBlock = SizeByBlock();
	}

	Size3 MemoryPageAdapter::BlockSize()
	{
		const std::size_t len = lvdReader.BlockSize();
		return Size3{ len,len,len };
	}

	int MemoryPageAdapter::Padding()
	{
		return lvdReader.GetBlockPadding();
	}

	Size3 MemoryPageAdapter::OriginalDataSize()
	{
		return lvdReader.OriginalDataSize();
	}

	Size3 MemoryPageAdapter::BlockDim()
	{
		return lvdReader.SizeByBlock();
	}

	const void* MemoryPageAdapter::GetPage(size_t pageID)
	{
		//const auto it = m_blockIdInCache.find(pageID);
		//if (it == m_blockIdInCache.end())
		//{
		//	// replace the last block in cache
		//	auto & lastCell = m_lruList.back();
		//	m_lruList.splice(m_lruList.begin(), m_lruList, --m_lruList.end());		// move last to head

		//	const auto newItr = m_blockIdInCache.insert(std::make_pair(pageID, m_lruList.begin()));
		//	if (lastCell.hashIter != m_blockIdInCache.end())
		//	{
		//		m_blockIdInCache.erase(lastCell.hashIter); // Unmapped old
		//	}

		//	lastCell.hashIter = newItr.first;		// Mapping new 

		//	auto d = (char*)m_volumeCache->GetBlockData(lastCell.blockCacheIndex);
		//	assert(d);

		//	lvdReader.ReadBlock(d, pageID);
		//	//SHOW_LIST_STATE
		//	return (unsigned char*)m_volumeCache->GetBlockData(lastCell.blockCacheIndex);
		//}
		//else
		//{
		//	m_lruList.splice(m_lruList.begin(), m_lruList, it->second);			// move the node that it->second points to the head.
		//	return (unsigned char*)m_volumeCache->GetBlockData(it->second->blockCacheIndex);
		//}

		return (void*)lvdReader.ReadBlock(pageID);
	}

}
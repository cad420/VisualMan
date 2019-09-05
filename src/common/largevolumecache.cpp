
#include "largevolumecache.h"
#include "error.h"
#include "cachepolicy.h"
#include "pluginloader.h"

#include <rapidjson/document.h>
#include <iostream>
#include <cassert>


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
	int MemoryPageAdapter::blockCoordinateToBlockId(int xBlock, int yBlock, int zBlock) const
	{
		//const auto size = lvdReader.SizeByBlock();
		const auto size = adapter->Get3DPageCount();
		const auto x = size.x, y = size.y, z = size.z;
		return zBlock * x * y + yBlock * x + xBlock;
	}

	void MemoryPageAdapter::Create()
	{
		const auto p = std::dynamic_pointer_cast<I3DBlockFilePluginInterface>(GetNextLevelCache());
		const int log = p->Get3DPageSizeInLog();
		const auto cacheSize = cacheDim * Size3(1 << log, 1 << log, 1 << log);
		assert(p);
		switch (log)
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


	int MemoryPageAdapter::GetLog() const
	{
		const auto p = std::dynamic_pointer_cast<const I3DBlockFilePluginInterface>(GetNextLevelCache());
		const int log = p->Get3DPageSizeInLog();
		return log;
	}


	void* MemoryPageAdapter::GetPageStorage_Implement(size_t pageID)
	{
		return m_volumeCache->GetBlockData(pageID);
	}

	MemoryPageAdapter::MemoryPageAdapter(const std::string& fileName) :
		//lvdReader(fileName),
		cacheDim(20, 20, 20)
	{
		const auto cap = fileName.substr(fileName.find_last_of('.'));
		auto p = PluginLoader::CreatePlugin<I3DBlockFilePluginInterface>(cap);

		if(p == nullptr)
		{
			throw std::runtime_error("Failed to load the plugin that is able to read " + cap + "file");
		}
		p->Open(fileName);
		SetDiskFileCache(std::move(p));
		SetCachePolicy(std::make_unique<LRUCachePolicy>());
	}

	void MemoryPageAdapter::SetDiskFileCache(std::shared_ptr<I3DBlockFilePluginInterface> diskCache)
	{
		SetNextLevelCache(diskCache);
		adapter = diskCache;
		Create();
	}

	Size3 MemoryPageAdapter::CPUCacheBlockSize() const
	{
		return Size3(1 << GetLog(), 1 << GetLog(), 1 << GetLog());
	}

	Size3 MemoryPageAdapter::CPUCacheSize() const
	{
		return CacheBlockDim() * (1 << GetLog());
	}

	Size3 MemoryPageAdapter::BlockSize()const
	{
		return adapter->Get3DPageSize();
	}

	int MemoryPageAdapter::Padding()const
	{
		return adapter->GetPadding();
	}

	Size3 MemoryPageAdapter::DataSizeWithoutPadding()const
	{
		return adapter->GetDataSizeWithoutPadding();
	}

	Size3 MemoryPageAdapter::BlockDim()const
	{
		return adapter->Get3DPageCount();
	}

}

#include "largevolumecache.h"
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
	AbstrCPUBlockCache::~AbstrCPUBlockCache()
	{

	}

	void CPUVolumeDataCache::createCache()
	{
		//constexpr auto maxDataBytes = (1ULL<<33);		// 8GB

		//const auto dataSize = Size();
		//const auto bytes = dataSize.x * dataSize.y * dataSize.z;


		//blockSize = ysl::Size3(1<<nLogBlockSize, 1 << nLogBlockSize, 1 << nLogBlockSize);
		//if(bytes > maxDataBytes)
		//{
		//	// Size of volume data larger than the defined capacity
		//	// [32, 32, 32] blocks are reserved.
		//	sizeByBlock = ysl::Size3(32, 32, 32);
		//	cacheSize = blockSize * sizeByBlock;
		//	
		//}else
		//{
		//	sizeByBlock = SizeByBlock();

		//	cacheSize = Size();
		//}
		auto p = new Cache(cacheSize.x, cacheSize.y, cacheSize.z, nullptr);
		if (!p)
		{
			std::cerr << "Can not allocate memory for cache\n";
			exit(0);
		}
		m_volumeCache.reset(p);
	}

	void CPUVolumeDataCache::initLRU()
	{
		for (auto i = std::size_t(0); i < totalCacheBlocks; i++)
			m_lruList.push_front(LRUListCell(i, m_blockIdInCache.end()));
	}

	CPUVolumeDataCache::CPUVolumeDataCache(const std::string& fileName,const Size3 & cacheBlockDim):AbstrBlockedVolumeDataCPUCache(cacheBlockDim),
	m_valid(true),
	lvdReader(fileName)
	{
		if (lvdReader.BlockSizeInLog() != nLogBlockSize)
		{
			m_valid = false;
			return;
		}
		createCache();
		initLRU();
		//sizeByBlock = SizeByBlock();
	}

	int CPUVolumeDataCache::BlockSize()
	{
		return lvdReader.BlockSize();
	}

	int CPUVolumeDataCache::Padding()
	{
		return lvdReader.BoundaryRepeat();
	}

	Size3 CPUVolumeDataCache::OriginalDataSize()
	{
		return lvdReader.OriginalDataSize();
	}

	Size3 CPUVolumeDataCache::BlockDim()
	{
		return lvdReader.SizeByBlock();
	}

	const unsigned char* CPUVolumeDataCache::ReadBlockDataFromCPUCache(int blockId)
	{
		//const auto it = m_blockIdInCache.find(blockId);
		//if (it == m_blockIdInCache.end())
		//{
		//	// replace the last block in cache
		//	auto & lastCell = m_lruList.back();
		//	m_lruList.splice(m_lruList.begin(), m_lruList, --m_lruList.end());		// move last to head

		//	const auto newItr = m_blockIdInCache.insert(std::make_pair(blockId, m_lruList.begin()));
		//	if (lastCell.hashIter != m_blockIdInCache.end())
		//	{
		//		m_blockIdInCache.erase(lastCell.hashIter); // Unmapped old
		//	}

		//	lastCell.hashIter = newItr.first;		// Mapping new 
		//	auto d = (char*)m_volumeCache->BlockData(lastCell.blockCacheIndex);
		//	assert(d);
		//	lvdReader.ReadBlock(d, blockId);
		//	//SHOW_LIST_STATE
		//	return m_volumeCache->BlockData(lastCell.blockCacheIndex);
		//}
		//else
		//{
		//	m_lruList.splice(m_lruList.begin(), m_lruList, it->second);			// move the node that it->second points to the head.
		//	return m_volumeCache->BlockData(it->second->blockCacheIndex);
		//}
		//return nullptr;

		return lvdReader.ReadBlock(blockId);
	}

}
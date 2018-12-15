#include "volume.h"
#include "../memory/blockarray.h"


#define SHOW_LIST_STATE																										\
		std::cout << "LRU State:" << std::endl;																				\
		for (auto it = m_lruList.begin(); it != m_lruList.end(); ++it)														\
		{																													\
			if (it->hashIter != LRUHash::iterator{})																		\
			std::cout << "[BlockID:" << it->hashIter->first << " -> CacheID:" << it->blockCacheIndex << "]--->";			\
		}																													\
		std::cout << std::endl;																								\


const unsigned char* LargeVolumeCache::blockData(int blockId)
{

	const auto it = m_blockIdInCache.find(blockId);
	if (it == m_blockIdInCache.end())
	{
		// replace the last block in cache
		auto & lastCell = m_lruList.back();
		std::cout << "The block:"<<blockId<<" is not in cache, read it from disk and put it at cache"<<lastCell.blockCacheIndex<<"\n";
		m_lruList.splice(m_lruList.begin(), m_lruList,--m_lruList.end());		// move last to head


		const auto newItr = m_blockIdInCache.insert(std::make_pair(blockId, m_lruList.begin()));
		if(lastCell.hashIter != LRUHash::iterator{})
		{
			std::cout << "Cache is full. Block "<< lastCell.hashIter->first<<" mapped at cache "<<lastCell.blockCacheIndex<<" must be replace\n";
			m_blockIdInCache.erase(lastCell.hashIter); // Unmapped old
		}

		lastCell.hashIter = newItr.first;		// Mapping new 
		m_blockVolumeReader.readBlock((char*)m_volumeCached.BlockData(lastCell.blockCacheIndex), blockId);
		SHOW_LIST_STATE
		return m_volumeCached.BlockData(lastCell.blockCacheIndex);
	}
	else
	{
		std::cout << "The block:" << blockId << " is cached at "<<it->second->blockCacheIndex<<", just use it\n";
		m_lruList.splice(m_lruList.begin(), m_lruList, it->second);			// move the node that it->second points to the head.
		SHOW_LIST_STATE
		return m_volumeCached.BlockData(it->second->blockCacheIndex);
	}


	return nullptr;
}

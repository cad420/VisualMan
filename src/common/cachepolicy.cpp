
#include "cachepolicy.h"

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
		LRUList().swap(m_lruList);
		for (auto i = std::size_t(0); i < cache->GetPhysicalPageCount(); i++)
			m_lruList.push_front(LRUListCell(i, m_blockIdInCache.end()));
	}
}
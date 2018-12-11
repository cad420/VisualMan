#include "volume.h"
#include "../memory/blockarray.h"



const unsigned char* LargeVolumeCache::blockData(int blockId)
{
	const auto it = m_blockIdInCache.find(blockId);
	if (it == m_blockIdInCache.end())
	{
		// replace the last block in cache
		const auto lastCell = m_lruList.back();
		m_lruList.splice(m_lruList.begin(), m_lruList,--m_lruList.end());		// move last to head
		m_blockIdInCache[blockId] = m_lruList.begin();
		m_blockIdInCache.erase(lastCell.hashIter); // Unmapped previous
		m_blockVolumeReader.readBlock((char*)m_volumeCached.BlockData(lastCell.blockCacheIndex), blockId);
		return m_volumeCached.BlockData(lastCell.blockCacheIndex);
	}
	else
	{
		m_lruList.splice(m_lruList.begin(), m_lruList, it->second);			// move the node that it->second points to the head.
		return m_volumeCached.BlockData(it->second->blockCacheIndex);
	}
	return nullptr;
}

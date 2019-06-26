
#ifndef _CACHEPOLICY_H_
#define _CACHEPOLICY_H_
#include <list>
#include <map>
#include "virtualmemorymanager.h"
#include "common.h"

namespace ysl
{
	class COMMON_EXPORT_IMPORT LRUCachePolicy :public AbstrCachePolicy
	{
		struct LRUListCell;
		using LRUList = std::list<LRUListCell>;
		using LRUHash = std::map<int, std::list<LRUListCell>::iterator>;
		struct LRUListCell
		{
			size_t storageID;
			LRUHash::iterator hashIter;
			LRUListCell(size_t index, LRUHash::iterator itr) :storageID{ index }, hashIter{ itr }{}
		};
		LRUList m_lruList;
		LRUHash	m_blockIdInCache;		// blockId---> (blockIndex,the position of blockIndex in list)
	public:
		LRUCachePolicy() = default;
		bool QueryPage(size_t pageID) override;
		void UpdatePage(size_t pageID) override;
		size_t QueryAndUpdate(size_t pageID) override;
	protected:
		void InitEvent(AbstrMemoryCache * cache) override;
	};
}
#endif
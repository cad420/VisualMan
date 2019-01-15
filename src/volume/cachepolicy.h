#ifndef _CACHEPOLICY_H_
#define _CACHEPOLICY_H_
#include <cstdint>
#include <list>
#include <unordered_map>

//#define DISABLE_COPY(Class) \
//	Class(const Class &)

//class CachePolicy
//{
//	int m_capacity;
//
//	void resize();
//public:
//	using CacheBlockId_t = uint32_t;
//
//	CachePolicy(int cap) :m_capacity(cap) {}
//
//	virtual CacheBlockId_t cacheBlock(int request) = 0;
//
//	int capacity()const { return m_capacity; }
//
//	//void setCapacity(int value) { m_capacity = value; }
//	void reserve(int value) { m_capacity = value; }
//
//	virtual ~CachePolicy();
//};
//
//class LRUCachePolicy:public CachePolicy
//{
//	struct LRUListCell;
//	using LRUHash = std::unordered_map<int, std::list<LRUListCell>::iterator>;
//	struct LRUListCell
//	{
//		int blockCacheIndex;
//		LRUHash::iterator hashIter;
//		LRUListCell(int index) :blockCacheIndex(index) {}
//	};
//
//	std::list<LRUListCell> m_lruList;
//	LRUHash	m_blockIdInCache;		// blockId---> (blockIndex,the position of blockIndex in queue)
//public:
//	LRUCachePolicy(int cap) :CachePolicy(cap)
//	{
//		
//	}
//
//	CacheBlockId_t cacheBlock(int request) override
//	{
//		
//	}
//};

#endif

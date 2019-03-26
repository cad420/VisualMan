
#ifndef _VIRTUALMEMORYMANAGER_H_
#define _VIRTUALMEMORYMANAGER_H_
#include "../volume/largevolumecache.h"
#include "../volume/virtualvolumehierachy.h"

namespace ysl
{
	namespace mem
	{
		//class AbstrMemoryObject;

		//struct MemoryBlockIndex
		//{
		//	
		//};

		//// 缺页策略
		//class MemoryFaultPolicy
		//{

		//public:
		//	MemoryFaultPolicy();
		//	std::vector<MemoryBlockIndex> Fault(const std::vector<MemoryBlockIndex>& missedBlockIndices);

		//	~MemoryFaultPolicy();

		//};

		//// 缺页处理类，用来实现交换算法
		//class MemoryFaultHandler
		//{
		//public:
		//	MemoryFaultHandler(MemoryFaultPolicy * policy);


		//	void TransferData(AbstrMemoryObject * phyMem, AbstrMemoryObject * virMem);

		//	//std::vector<MemoryBlockIndex> UpdatePageTable(const std::vector<MemoryBlockIndex>& missedBlockIndices);
		//};


		//// 内存模型基类
		//class AbstrMemoryObject
		//{
		//public:
		//	AbstrMemoryObject(MemoryFaultHandler * handler);
		//	virtual std::vector<MemoryBlockIndex> CaptureMemFault()const;


		//};


		//// 一个聚合类，主要用来建立联系。
		//class AbstrVirtualMemoryManager
		//{
		//public:
		//	AbstrVirtualMemoryManager(AbstrMemoryObject * phyMem, AbstrMemoryObject * virMem,MemoryFaultHandler * handler);
		//	virtual void Schedule();
		//};


	}
}



#endif

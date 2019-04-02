
#ifndef _VIRTUALMEMORYMANAGER_H_
#define _VIRTUALMEMORYMANAGER_H_
#include "../volume/largevolumecache.h"
#include "../volume/virtualvolumehierachy.h"

namespace ysl
{
	namespace mem
	{
		class AbstrMemoryObject;

		struct MemAddrIndex
		{
			
		};

		// 缺页策略
		class MemoryFaultPolicy
		{

		public:
			MemoryFaultPolicy();
			std::vector<MemAddrIndex> Fault(const std::vector<MemAddrIndex>& missedBlockIndices);

			~MemoryFaultPolicy();

		};

		// 缺页处理类，用来实现交换算法

		class MemoryFaultHandler
		{
		public:
			MemoryFaultHandler(MemoryFaultPolicy * policy);


			void TransferData(AbstrMemoryObject * phyMem, AbstrMemoryObject * virMem);

			//std::vector<MemoryBlockIndex> UpdatePageTable(const std::vector<MemoryBlockIndex>& missedBlockIndices);
		};


		// 内存模型基类
		class AbstrMemoryObject
		{
			public:
				AbstrMemoryObject(MemoryFaultHandler * handler);
				virtual std::vector<MemAddrIndex> CaptureMemFault()const;
		};


		class AbstrServerMemoryObject:public AbstrMemoryObject
		{
			public:
		};

		class AbstrVirtualMemoryObject:public AbstrServerMemoryObject
		{
			public:
		};

		class AbstrClientMemoryObject:public AbstrMemoryObject
		{
			
		};


		// 一个聚合类，主要用来建立联系。
		class AbstrVirtualMemoryManager
		{
		public:
			static AbstrVirtualMemoryObject * CreateVirtualMemoryObject(AbstrClientMemoryObject * clientMem, AbstrServerMemoryObject * serverMem);;
		};

	}
}



#endif

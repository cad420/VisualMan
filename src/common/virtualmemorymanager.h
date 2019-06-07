
#ifndef _VIRTUALMEMORYMANAGER_H_
#define _VIRTUALMEMORYMANAGER_H_
#include "../volume/largevolumecache.h"
#include "../volume/virtualvolumehierachy.h"
#include <vector>

namespace ysl
{
	namespace mem
	{
		class AbstrMemoryObject;

		struct MemoryAddress
		{

		};

		// 缺页策略
		class MemoryFaultPolicy
		{

		public:
			MemoryFaultPolicy();
			std::vector<MemoryAddress> Fault(const std::vector<MemoryAddress>& missedBlockIndices);

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

		class AbstrMemoryManager;

		class AbstrMemoryObject
		{
			AbstrMemoryManager * manager;
		public:
			AbstrMemoryObject(MemoryFaultHandler * handler);
			virtual std::vector<MemoryAddress> CaptureMemFault()const;
			virtual void * Fetch(const MemoryAddress & addr);
			AbstrMemoryManager * Manager();
			virtual ~AbstrMemoryObject() = 0;
			friend AbstrMemoryManager;
		};


		class AbstrServerMemoryObject :public AbstrMemoryObject
		{
			public:
		};

		class AbstrClientMemoryObject :public AbstrMemoryObject
		{
			public:
		};

		class AbstrVirtualMemoryObject :public AbstrServerMemoryObject
		{
		public:
			virtual MemoryAddress Translate(const MemoryAddress & va);
			void SetExplicitUpdate(bool enable);
			void ResetFaults();
		    std::vector<MemoryAddress> Faults()const;
		};

		class AbstrMemoryManager:public AbstrVirtualMemoryObject			// Page table
		{
		public:
			AbstrMemoryManager(AbstrClientMemoryObject * clientMem, AbstrServerMemoryObject * serverMem);

		};

	}
}



#endif

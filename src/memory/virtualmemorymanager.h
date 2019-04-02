
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

		// ȱҳ����
		class MemoryFaultPolicy
		{

		public:
			MemoryFaultPolicy();
			std::vector<MemAddrIndex> Fault(const std::vector<MemAddrIndex>& missedBlockIndices);

			~MemoryFaultPolicy();

		};

		// ȱҳ�����࣬����ʵ�ֽ����㷨

		class MemoryFaultHandler
		{
		public:
			MemoryFaultHandler(MemoryFaultPolicy * policy);


			void TransferData(AbstrMemoryObject * phyMem, AbstrMemoryObject * virMem);

			//std::vector<MemoryBlockIndex> UpdatePageTable(const std::vector<MemoryBlockIndex>& missedBlockIndices);
		};


		// �ڴ�ģ�ͻ���
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


		// һ���ۺ��࣬��Ҫ����������ϵ��
		class AbstrVirtualMemoryManager
		{
		public:
			static AbstrVirtualMemoryObject * CreateVirtualMemoryObject(AbstrClientMemoryObject * clientMem, AbstrServerMemoryObject * serverMem);;
		};

	}
}



#endif

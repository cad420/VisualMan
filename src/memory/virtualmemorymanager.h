
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

		//// ȱҳ����
		//class MemoryFaultPolicy
		//{

		//public:
		//	MemoryFaultPolicy();
		//	std::vector<MemoryBlockIndex> Fault(const std::vector<MemoryBlockIndex>& missedBlockIndices);

		//	~MemoryFaultPolicy();

		//};

		//// ȱҳ�����࣬����ʵ�ֽ����㷨
		//class MemoryFaultHandler
		//{
		//public:
		//	MemoryFaultHandler(MemoryFaultPolicy * policy);


		//	void TransferData(AbstrMemoryObject * phyMem, AbstrMemoryObject * virMem);

		//	//std::vector<MemoryBlockIndex> UpdatePageTable(const std::vector<MemoryBlockIndex>& missedBlockIndices);
		//};


		//// �ڴ�ģ�ͻ���
		//class AbstrMemoryObject
		//{
		//public:
		//	AbstrMemoryObject(MemoryFaultHandler * handler);
		//	virtual std::vector<MemoryBlockIndex> CaptureMemFault()const;


		//};


		//// һ���ۺ��࣬��Ҫ����������ϵ��
		//class AbstrVirtualMemoryManager
		//{
		//public:
		//	AbstrVirtualMemoryManager(AbstrMemoryObject * phyMem, AbstrMemoryObject * virMem,MemoryFaultHandler * handler);
		//	virtual void Schedule();
		//};


	}
}



#endif


#ifndef _GPUPAGETABLE_H_
#define _GPUPAGETABLE_H_
#include "abstrgpucache.h"
//#include "../volume/largevolumecache.h"
namespace ysl
{
	class GPUPageTableDataCache :public AbstrGPUCache
	{
		const Size3 & cacheSize;
	public:
		GPUPageTableDataCache(const Size3 & cacheSize, void * data);
	};
}

#endif
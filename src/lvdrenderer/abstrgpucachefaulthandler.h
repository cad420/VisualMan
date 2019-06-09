
#ifndef _ABSTRGPUCACHEFAULTHANDLER_H_
#define _ABSTRGPUCACHEFAULTHANDLER_H_
#include <largevolumecache.h>

namespace ysl
{
	class AbstrGPUCacheFaultHandler
	{
	public:
		AbstrGPUCacheFaultHandler() = default;
		virtual std::vector<VirtualMemoryBlockIndex> CaptureCacheFault() = 0;
		virtual void Reset() = 0;
		virtual ~AbstrGPUCacheFaultHandler() = default;
	};

}

#endif
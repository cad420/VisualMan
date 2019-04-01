
#ifndef _ABSTRGPUCACHEBLOCKMANAGER_H_
#define _ABSTRGPUCACHEBLOCKMANAGER_H_
#include "../gpucacheblockmanager.h"
#include "abstrgpucachefaulthandler.h"


namespace ysl
{
	class GPUVolumeDataCache;
	class CPUVolumeDataCache;

	class AbstrGPUCacheBlockManager
	{
	protected:
		VirtualMemoryManager * vmManager;
		AbstrGPUCacheFaultHandler * gcmHandler;
	public:
		AbstrGPUCacheBlockManager(VirtualMemoryManager * vmm, AbstrGPUCacheFaultHandler * gcm) :vmManager(vmm), gcmHandler(gcm) {}
		virtual bool TransferData(GPUVolumeDataCache* dest, CPUVolumeDataCache* src) = 0;
		virtual ~AbstrGPUCacheBlockManager() {}
	};
}

#endif
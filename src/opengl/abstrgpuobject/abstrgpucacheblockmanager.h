
#ifndef _ABSTRGPUCACHEBLOCKMANAGER_H_
#define _ABSTRGPUCACHEBLOCKMANAGER_H_
#include "abstrgpucachefaulthandler.h"
#include "../virtualvolumehierachy.h"


namespace ysl
{
	class GPUVolumeDataCache;
	class CPUVolumeDataCache;

	class AbstrGPUCacheBlockManager
	{
	protected:
		PageTableManager * vmManager;
		AbstrGPUCacheFaultHandler * gcmHandler;
	public:
		AbstrGPUCacheBlockManager(PageTableManager* vmm, AbstrGPUCacheFaultHandler* gcm);

		virtual bool TransferData(GPUVolumeDataCache* dest, CPUVolumeDataCache* src) = 0;
		virtual ~AbstrGPUCacheBlockManager();
	};
}

#endif
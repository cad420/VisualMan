
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
		std::shared_ptr<PageTableManager> vmManager;
		std::shared_ptr<AbstrGPUCacheFaultHandler> gcmHandler;
	public:
		AbstrGPUCacheBlockManager(std::shared_ptr<PageTableManager> vmm, std::shared_ptr<AbstrGPUCacheFaultHandler> gcm);
		virtual bool TransferData(GPUVolumeDataCache* dest, CPUVolumeDataCache* src) = 0;
		virtual ~AbstrGPUCacheBlockManager();
	};
}

#endif
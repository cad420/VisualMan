
#include "abstrgpucacheblockmanager.h"

namespace ysl
{
	AbstrGPUCacheBlockManager::
	AbstrGPUCacheBlockManager(std::shared_ptr<PageTableManager> vmm, std::shared_ptr<AbstrGPUCacheFaultHandler> gcm):
	vmManager(std::move(vmm)), gcmHandler(std::move(gcm))
	{
	}

	AbstrGPUCacheBlockManager::~AbstrGPUCacheBlockManager()
	{
	}
}

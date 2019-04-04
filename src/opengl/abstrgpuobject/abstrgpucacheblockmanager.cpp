
#include "abstrgpucacheblockmanager.h"

namespace ysl
{
	AbstrGPUCacheBlockManager::
	AbstrGPUCacheBlockManager(PageTableManager* vmm, AbstrGPUCacheFaultHandler* gcm): vmManager(vmm), gcmHandler(gcm)
	{
	}

	AbstrGPUCacheBlockManager::~AbstrGPUCacheBlockManager()
	{
	}
}

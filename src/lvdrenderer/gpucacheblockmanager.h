
#ifndef _GPUCACHEMANAGER_H_
#define _GPUCACHEMANAGER_H_

#include "texture.h"
#include "abstrgpucacheblockmanager.h"

namespace ysl
{
	//class PageTableManager;
	//class AbstrGPUCacheFaultHandler;
	//class GPUVolumeDataCache;
	//class CPUVolumeDataCache;

	class PingPongTransferManager:public AbstrGPUCacheBlockManager
	{
		std::shared_ptr<OpenGLBuffer> pbo[2];
		void InitPingPongSwapPBO();
	public:
		PingPongTransferManager(std::shared_ptr<PageTableManager> vmm, std::shared_ptr<AbstrGPUCacheFaultHandler> gcm);
		bool TransferData(GPUVolumeDataCache * dest, MemoryPageAdapter* src) override;
		~PingPongTransferManager();
	};

}

#endif

#ifndef _GPUCACHEMANAGER_H_
#define _GPUCACHEMANAGER_H_

#include "texture.h"
#include "abstrgpuobject/abstrgpucacheblockmanager.h"

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
		PingPongTransferManager(PageTableManager * vmm, AbstrGPUCacheFaultHandler * gcm);
		bool TransferData(GPUVolumeDataCache* dest, CPUVolumeDataCache* src) override;
		~PingPongTransferManager();
	};

}

#endif
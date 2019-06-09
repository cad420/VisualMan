
#ifndef _LODAGGREGATE_H_
#define _LODAGGREGATE_H_

#include <memory>
#include <string>
#include <geometry.h>

namespace ysl
{

	struct SHADERBINDINGPOINT
	{
		int PAGE_TABLE_CACHE_BINDING_POINT;
		int HASH_TABLE_BUFFER_BINDING_POINT;
		int FAULT_TABLE_BUFFER_BINDING_POINT;
		int ATOMIC_COUNTER_BINDING_POINT;
	};

	class HashBasedGPUCacheFaultHandler;
	class PingPongTransferManager;
	class GPUVolumeDataCache;
	class CPUVolumeDataCache;
	class PageTableManager;

	class LODAggregate
	{
		std::shared_ptr<HashBasedGPUCacheFaultHandler> cacheFaultHandler; // Belong to Client-end memory
		std::shared_ptr<PingPongTransferManager> pingpongTransferManager;
		std::shared_ptr<GPUVolumeDataCache> texCache;			 // Client-end memory
		std::shared_ptr<CPUVolumeDataCache> largeVolumeCache;
		std::shared_ptr<PageTableManager> pageTableManager;		// Server-end memory
		void InitGPUPageTableBuffer();
		void InitGPUBlockCacheTexture(const Size3 & blockDim);		// texCache
	public:
		LODAggregate(const std::string & fileName,const Size3 & blockDim);

		std::shared_ptr<GPUVolumeDataCache> GPUCache()const;
		std::shared_ptr<CPUVolumeDataCache> CPUCache()const;

		Size3 PageTableSize()const;
		Size3 OriginalDataSize()const;
		Size3 BlockSize()const;

		Size3 CPUBlockDim()const;
		Size3 CPUCacheSize()const;
		Size3 GPUBlockDim()const;
		Size3 GPUCacheSize()const;
		int Padding()const;


		bool CaptureAndHandleCacheFault();
		void Bind(const SHADERBINDINGPOINT & bp);
	};

}


#endif
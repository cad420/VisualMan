
#ifndef _GPUCACHEBLOCKMANAGER_H_
#define _GPUCACHEBLOCKMANAGER_H_
#include "texture.h"
#include "../mathematics/geometry.h"
#include "../volume/virtualvolumehierachy.h"
#include "abstrgpuobject/abstrgpucacheblockmanager.h"
#include "abstrgpuobject/abstrgpucache.h"

namespace ysl
{

	/**
	 * \brief 
	 * 
	 * \note This class should not be derived from OpenGLTexture
	 */


	class GPUVolumeDataCache :public AbstrGPUCache
	{
		const Size3 & cacheSize;
	public:
		enum CacheExternalDataFormat { Red = OpenGLTexture::RED };
		enum CacheExternalDataType { UInt8 = OpenGLTexture::UInt8 };
		enum CacheInternalDataFormat { Red8 = OpenGLTexture::R8 };
		GPUVolumeDataCache(const Size3 & cacheSize, void * data);
	};


	class HashBasedGPUCacheFaultHandler:public AbstrGPUCacheFaultHandler
	{
		std::shared_ptr<OpenGLBuffer> bufMissedHash;
		std::shared_ptr<OpenGLBuffer> bufMissedTable;
		std::shared_ptr<OpenGLBuffer> atomicCounter;

		void InitHashBuffer(int blockCount);
		void InitMissTableBuffer();
		void InitGPUAtomicCounter();

		void ResetBlockExistsHash();
		void ResetMissedBlockVector();

		const int capacity;
		const Size3 dim;

	public:
		HashBasedGPUCacheFaultHandler(int capacity,const Size3 & dim);
		std::vector<VirtualMemoryBlockIndex> CaptureCacheFault() override;
		void Reset() override;
		~HashBasedGPUCacheFaultHandler();

	};


	class PingPongTransferManager:public AbstrGPUCacheBlockManager
	{
		std::shared_ptr<OpenGLBuffer> pbo[2];
		void InitPingPongSwapPBO();
	public:
		PingPongTransferManager(VirtualMemoryManager * vmm, AbstrGPUCacheFaultHandler * gcm);

		bool TransferData(GPUVolumeDataCache* dest, CPUVolumeDataCache* src) override;
		~PingPongTransferManager();
	};

}


#endif
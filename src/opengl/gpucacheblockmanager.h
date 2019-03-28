
#ifndef _GPUCACHEBLOCKMANAGER_H_
#define _GPUCACHEBLOCKMANAGER_H_
#include "texture.h"
#include "../mathematics/geometry.h"
#include "../volume/virtualvolumehierachy.h"

namespace ysl
{

	class GPUCache :public OpenGLTexture
	{
		const Size3 & cacheSize;
	public:
		enum CacheExternalDataFormat { Red = OpenGLTexture::RED };
		enum CacheExternalDataType { UInt8 = OpenGLTexture::UInt8 };
		enum CacheInternalDataFormat { Red8 = OpenGLTexture::R8 };
		GPUCache(const Size3 & cacheSize, void * data);



	};


	class AbstrGPUCacheFaultHandler
	{
	public:
		AbstrGPUCacheFaultHandler() = default;
		virtual std::vector<VirtualMemoryBlockIndex> CaptureCacheFault() = 0;
		virtual void Reset() = 0;
		virtual ~AbstrGPUCacheFaultHandler() = default;
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


	class AbstrGPUCacheBlockManager
	{
	protected:
		VirtualMemoryManager * vmManager;
		AbstrGPUCacheFaultHandler * gcmHandler;
	public:
		AbstrGPUCacheBlockManager(VirtualMemoryManager * vmm, AbstrGPUCacheFaultHandler * gcm):vmManager(vmm),gcmHandler(gcm){}
		virtual bool TransferData(GPUCache* dest, CPUVolumeDataCache* src) = 0;
		virtual ~AbstrGPUCacheBlockManager(){}
	};

	class PingPongTransferManager:public AbstrGPUCacheBlockManager
	{
		std::shared_ptr<OpenGLBuffer> pbo[2];
		void InitPingPongSwapPBO();
	public:
		PingPongTransferManager(VirtualMemoryManager * vmm, AbstrGPUCacheFaultHandler * gcm);

		bool TransferData(GPUCache* dest, CPUVolumeDataCache* src) override;
		~PingPongTransferManager();
	};


}


#endif
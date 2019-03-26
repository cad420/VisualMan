
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
		GPUCache(const Size3 & cacheSize, void * data, CacheExternalDataFormat extFmt, CacheExternalDataFormat extType, CacheInternalDataFormat intFmt);
	};


	class AbstraGPUCacheMissHandler
	{
	public:
		AbstraGPUCacheMissHandler();
		virtual std::vector<VirtualMemoryBlockIndex> CaptureCacheMiss() = 0;
		virtual void Reset() = 0;
		virtual ~AbstraGPUCacheMissHandler();
	};



	class HashBasedGPUCacheMissHandler:public AbstraGPUCacheMissHandler
	{
		std::shared_ptr<OpenGLBuffer> bufMissedHash;
		std::shared_ptr<OpenGLBuffer> bufMissedTable;
		std::shared_ptr<OpenGLBuffer> atomicCounter;

		void InitHashBuffer(int blockCount);
		void InitMissTableBuffer();
		void InitGPUAtomicCounter();

		void ResetBlockExistsHash();
		void ResetMissedBlockVector();

	public:
		HashBasedGPUCacheMissHandler(int blockCount);
		std::vector<VirtualMemoryBlockIndex> CaptureCacheMiss() override;
		void Reset() override;
	};


	class AbstraGPUCacheBlockManager
	{
	protected:
		VirtualMemoryManager * vmManager;
		AbstraGPUCacheMissHandler * gcmHandler;
	public:
		AbstraGPUCacheBlockManager(VirtualMemoryManager * vmm, AbstraGPUCacheMissHandler * gcm):vmManager(vmm),gcmHandler(gcm){}
		virtual void TransferData(GPUCache * dest, const CPUVolumeDataCache * src) = 0;
		virtual ~AbstraGPUCacheBlockManager(){}
	};




	class PingPongTransferManager:public AbstraGPUCacheBlockManager
	{
		std::shared_ptr<OpenGLBuffer> pbo[2];
		void InitPingPongSwapPBO();
	public:
		PingPongTransferManager(VirtualMemoryManager * vmm, AbstraGPUCacheMissHandler * gcm);
		void TransferData(GPUCache * dest, const CPUVolumeDataCache * src) override;
		~PingPongTransferManager();
	};



}


#endif
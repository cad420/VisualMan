
#ifndef _GPUVOLUMEDATACACHE_H_
#define _GPUVOLUMEDATACACHE_H_

#include <geometry.h>
#include "abstrgpucache.h"

namespace ysl
{
	class GPUVolumeDataCache :public AbstrGPUCache
	{
		const Size3 blockDim;
		const Size3 blockSize;
		const Size3 cacheSize;
	public:
		enum CacheExternalDataFormat { Red = OpenGLTexture::RED };
		enum CacheExternalDataType { UInt8 = OpenGLTexture::UInt8 };
		enum CacheInternalDataFormat { Red8 = OpenGLTexture::R8 };
		GPUVolumeDataCache(const Size3 & blockDim, const Size3 & blockSize, void * data);
		Size3 BlockDim()const;
		Size3 CacheSize()const;
		Size3 BlockSize()const;
	};

}

#endif

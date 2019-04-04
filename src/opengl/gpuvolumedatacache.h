
#ifndef _GPUVOLUMEDATACACHE_H_
#define _GPUVOLUMEDATACACHE_H_

#include "../mathematics/geometry.h"
#include "abstrgpuobject/abstrgpucache.h"

namespace ysl
{
	class GPUVolumeDataCache :public AbstrGPUCache
	{
		const Size3 & cacheSize;
	public:
		enum CacheExternalDataFormat { Red = OpenGLTexture::RED };
		enum CacheExternalDataType { UInt8 = OpenGLTexture::UInt8 };
		enum CacheInternalDataFormat { Red8 = OpenGLTexture::R8 };
		GPUVolumeDataCache(const Size3 & cacheSize, void * data);
	};
}

#endif

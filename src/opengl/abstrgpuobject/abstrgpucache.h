
#ifndef _ABSTRGPUCACHE_H_
#define _ABSTRGPUCACHE_H_

#include "../texture.h"

namespace ysl
{

	class AbstrGPUCache :public OpenGLTexture
	{
	public:
		AbstrGPUCache(TextureTarget target,
			FilterMode min = FilterMode::Linear,
			FilterMode mag = FilterMode::Linear,
			WrapMode rWrapMode = WrapMode::ClampToEdge,
			WrapMode sWrapMode = WrapMode::ClampToEdge,
			WrapMode tWrapMode = WrapMode::ClampToEdge) :OpenGLTexture(target, min, mag, rWrapMode, sWrapMode, tWrapMode) {}
		virtual ~AbstrGPUCache(){}
	};

}

#endif
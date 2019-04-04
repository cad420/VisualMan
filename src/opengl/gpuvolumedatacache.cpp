
#include "gpuvolumedatacache.h"

namespace ysl
{
	GPUVolumeDataCache::GPUVolumeDataCache(const Size3& cacheSize, void * data) :
		AbstrGPUCache(OpenGLTexture::Texture3D
			, OpenGLTexture::Linear
			, OpenGLTexture::Linear
			, OpenGLTexture::ClampToEdge
			, OpenGLTexture::ClampToEdge
			, OpenGLTexture::ClampToEdge
		), cacheSize(cacheSize)
	{
		Bind();
		SetData(OpenGLTexture::InternalFormat::R8, OpenGLTexture::ExternalDataFormat::RED, OpenGLTexture::UInt8, cacheSize.x, cacheSize.y, cacheSize.z, data);
		Unbind();
	}
}
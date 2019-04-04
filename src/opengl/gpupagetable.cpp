
#include "gpupagetable.h"

namespace ysl
{
	GPUPageTableDataCache::GPUPageTableDataCache(const Size3& cacheSize, void* data) :
		AbstrGPUCache(Texture3D, Linear, Linear, OpenGLTexture::ClampToEdge, OpenGLTexture::ClampToEdge, OpenGLTexture::ClampToEdge
		), cacheSize(cacheSize)
	{
		Bind();
		SetData(OpenGLTexture::RGBA32UI,
			OpenGLTexture::ExternalDataFormat::RGBAInteger,
			OpenGLTexture::ExternalDataType::UInt32,
			cacheSize.x,
			cacheSize.y,
			cacheSize.z,
			data);
		Unbind();
	}
}
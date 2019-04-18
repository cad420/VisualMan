
#include "gpuvolumedatacache.h"

namespace ysl
{
	GPUVolumeDataCache::GPUVolumeDataCache(const Size3 & blockDim,const Size3 & blockSize,void * data) :
		AbstrGPUCache(OpenGLTexture::Texture3D
			, OpenGLTexture::Linear
			, OpenGLTexture::Linear
			, OpenGLTexture::ClampToEdge
			, OpenGLTexture::ClampToEdge
			, OpenGLTexture::ClampToEdge
		), blockDim(blockDim), blockSize(blockSize),cacheSize(blockDim*blockSize)
	{
		Bind();
		SetData(OpenGLTexture::InternalFormat::R8, 
			OpenGLTexture::ExternalDataFormat::RED, 
			OpenGLTexture::UInt8, 
			cacheSize.x,
			cacheSize.y, 
			cacheSize.z, 
			data);
		Unbind();
	}

	Size3 GPUVolumeDataCache::BlockDim() const
	{
		return blockDim;
	}

	Size3 GPUVolumeDataCache::CacheSize() const
	{
		return cacheSize;
	}

	Size3 GPUVolumeDataCache::BlockSize() const
	{
		return blockSize;
	}
}

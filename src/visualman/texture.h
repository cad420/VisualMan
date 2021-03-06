
#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "graphictype.h"
#include "bufferobject.h"
#include <VMGraphics/spectrum.h>
#include <vector>

namespace vm
{


class TexParams
{
public:
	TexParams() = default;
	void Apply();
	TexFilter MinFilter() const { return minFilter; }
	TexFilter MaxFilter() const { return maxFilter; }
	TexWrap WrapS() const { return wrapS; }
	TexWrap WrapT() const { return wrapT; }
	TexWrap WrapR() const { return wrapR; }

	void SetMinFilter( TexFilter minFilter ) { this->minFilter = minFilter; }
	void SetMaxFilter( TexFilter maxFilter ) { this->maxFilter = maxFilter; }
	void SetWarpR( TexWrap wrapR ) { this->wrapR = wrapR; }
	void SetWarpS( TexWrap wrapS ) { this->wrapS = wrapS; }
	void SetWarpT( TexWrap wrapT ) { this->wrapT = wrapT; }

private:
	TexFilter minFilter = TPF_LINEAR;
	TexFilter maxFilter = TPF_LINEAR;
	TexWrap wrapS = TPW_CLAMP_TO_EDGE;
	TexWrap wrapT = TPW_CLAMP_TO_EDGE;
	TexWrap wrapR = TPW_CLAMP_TO_EDGE;
};

/**
		 * \brief A Texture is created according to the parameters in this class.
		 *
		 * A data member should be added for the data source of the texture to created
		 *
		 */
class TexCreateParams
{
public:
	void SetTextureTarget( TextureTarget target ) { this->target = target; }
	TextureTarget GetTextureTarget() const { return target; }

	void SetTextureFormat( TextureFormat format ) { this->format = format; }
	TextureFormat GetTextureFormat() const { return this->format; }

	VMRef<BufferObject> GetBufferObject() { return bufferObject; }
	void SetBufferObject( VMRef<BufferObject> bo ) { bufferObject = std::move( bo ); }

	void EnableBorder( bool enable ) { border = enable; }
	bool IsBorder() const { return border; }
	void EnableMipMap( bool enable ) { this->mipMap = enable; }
	bool IsMipMap() const { return mipMap; }

	void SetSize( int w, int h, int d )
	{
		this->w = w;
		this->h = h;
		this->d = d;
	}
	int Width() const { return w; }
	int Height() const { return h; }
	int Depth() const { return d; }

private:
	// Default parameters
	TextureTarget target = TD_TEXTURE_2D;
	TextureFormat format = TF_RGBA;
	VMRef<BufferObject> bufferObject;

	bool border = false;
	bool mipMap = false;
	int nSamples = 0;
	int w = 0, h = 0, d = 0;
};

int GetBaseFormatBySizedFormat( TextureFormat sizedFormat );
int GetTypeBySizedFormat( TextureFormat sizedFormat );

class VISUALMAN_EXPORT_IMPORT Texture
{
public:
	Texture()
	{
		texParams = MakeVMRef<TexParams>();
	}

	~Texture();

	bool CreateTexture();

	bool CreateTexture( TextureTarget texDimension,
						TextureFormat texFormat,
						int w, int h, int d,
						bool border,
						VMRef<BufferObject> bufferObject,
						int sampels );

	void DestroyTexture();

	VMRef<TexParams> GetTextureParams() { return texParams; }
	VMRef<const TexParams> GetTextureParams() const { return texParams; }
	void SetTextureParams( VMRef<TexParams> params ) { texParams = std::move( params ); }

	VMRef<TexCreateParams> GetSetupParams() { return createParams; }
	VMRef<const TexCreateParams> GetSetupParams() const { return createParams; }

	void SetSetupParams( VMRef<TexCreateParams> params );

	VMRef<BufferObject> GetBufferObject() { return bufferObject; }
	VMRef<const BufferObject> GetBufferObject() const { return bufferObject; }

	void SaveTextureAs( const std::string &fileName );

	int Width() const { return w; }
	int Height() const { return h; }
	int Depth() const { return d; }

	TextureTarget GetTextureTarget() const { return target; }
	TextureFormat GetTextureFormat() const { return format; }

	unsigned int Handle() const { return handle; }

	bool GetTextureDataDirty() const { return this->dirty; };
	void SetTextureDataDirty( bool dirty ) { this->dirty = dirty; }

	/**
			 * \brief Set texture sub data given by \a data
			 */
	void SetSubTextureData( const void *data, ImageFormat imageFormat, ImageType imageType, int xOffset, int yOffset, int zOffset, int w, int h, int d );
	void SetSubTextureData( const void *data, int xOffset, int yOffset, int zOffset, int w, int h, int d );

	void SetSubTextureData( ImageFormat imageFormat, ImageType imageType, int xOffset, int yOffset, int zOffset, int w, int h, int d );
	void SetSubTextureData( int xOffset, int yOffset, int zOffset, int w, int h, int d );

	void SetSubTextureDataUsePBO( ImageFormat imageFormat, ImageType imageType, int xOffset, int yOffset, int zOffset, int w, int h, int d );
	void SetSubTextureDataUsePBO( int xOffset, int yOffset, int zOffset, int w, int h, int d );

private:
	// std::unique_ptr<BufferObject> pixelBufferObject; this is used to swap data between texture and memory

	VMRef<BufferObject> bufferObject;
	VMRef<TexCreateParams> createParams = nullptr;
	VMRef<TexParams> texParams = nullptr;
	bool dirty = true;

	TextureTarget target = TD_TEXTURE_2D;
	TextureFormat format = TF_RGBA;
	int w = 0;
	int h = 0;
	int d = 0;
	int samples = 0;
	bool border = false;
	bool mipMap = false;
	unsigned int handle = 0;
};

VMRef<Texture> VISUALMAN_EXPORT_IMPORT MakeVolumeTexture( const std::string &fileName, size_t x, size_t y, size_t z );
VMRef<Texture> VISUALMAN_EXPORT_IMPORT MakeImageTexuture( const std::string &fileName );
VMRef<Texture> VISUALMAN_EXPORT_IMPORT MakeTransferFunction1DTexture( const std::string &fileName );
VMRef<Texture> VISUALMAN_EXPORT_IMPORT MakeTransferFunction1DTexture( const std::vector<Color> &colors );
VMRef<Texture> VISUALMAN_EXPORT_IMPORT MakePreIntegratedTransferFunction2DTexture( const std::string &fileName );
}  // namespace vm

#endif

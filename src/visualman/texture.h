
#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "graphictype.h"
#include "bufferobject.h"
#include "spectrum.h"

namespace ysl
{
	namespace vm
	{
		class TexParams
		{
		public:
			TexParams() = default;
			void Apply();
			TexFilter MinFilter()const { return minFilter; }
			TexFilter MaxFilter()const { return maxFilter; }
			TexWrap WrapS()const { return wrapS; }
			TexWrap WrapT()const { return wrapT; }
			TexWrap WrapR()const { return wrapR; }

			void SetMinFilter(TexFilter minFilter) { this->minFilter = minFilter; }
			void SetMaxFilter(TexFilter maxFilter) { this->maxFilter = maxFilter; }
			void SetWarpR(TexWrap wrapR) { this->wrapR = wrapR; }
			void SetWarpS(TexWrap wrapS) { this->wrapS = wrapS; }
			void SetWarpT(TexWrap wrapT) { this->wrapT = wrapT; }
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

			void SetTextureTarget(TextureTarget target) { this->target = target; }
			TextureTarget GetTextureTarget()const { return target; }

			void SetTextureFormat(TextureFormat format) { this->format = format; }
			TextureFormat GetTextureFormat()const { return this->format; }

			Ref<BufferObject> GetBufferObject() { return bufferObject; }
			void SetBufferObject(Ref<BufferObject> bo) { bufferObject = std::move(bo); }


			void EnableBorder(bool enable) { border = enable; }
			bool IsBorder()const { return border; }
			void EnableMipMap(bool enable) { this->mipMap = enable; }
			bool IsMipMap()const { return mipMap; }

			void SetSize(int w, int h, int d) { this->w = w; this->h = h; this->d = d; }
			int Width()const { return w; }
			int Height()const { return h; }
			int Depth()const { return d; }


		private:
			// Default parameters
			TextureTarget target = TD_TEXTURE_2D;
			TextureFormat format = TF_RGBA;
			Ref<BufferObject> bufferObject;

			bool border = false;
			bool mipMap = false;
			int nSamples = 0;
			int w = 0, h = 0, d = 0;

		};

		int GetBaseFormatBySizedFormat(TextureFormat sizedFormat);
		int GetTypeBySizedFormat(TextureFormat sizedFormat);


		class VISUALMAN_EXPORT_IMPORT  Texture
		{
		public:
			Texture()
			{
				texParams = MakeRef<TexParams>();
			}

			~Texture();

			bool CreateTexture();

			bool CreateTexture(TextureTarget texDimension,
			                   TextureFormat texFormat,
			                   int w, int h, int d,
			                   bool border,
			                   Ref<BufferObject> bufferObject,
			                   int sampels);

			void DestroyTexture();

			Ref<TexParams> GetTextureParams() { return texParams; }
			Ref<const TexParams> GetTextureParams()const { return texParams; }
			void SetTextureParams(Ref<TexParams> params) { texParams = std::move(params); }

			Ref<TexCreateParams> GetSetupParams() { return createParams; }
			Ref<const TexCreateParams> GetSetupParams()const { return createParams; }

			void SetSetupParams(Ref<TexCreateParams> params);

			Ref<BufferObject> GetBufferObject() { return bufferObject; }
			Ref<const BufferObject> GetBufferObject() const { return bufferObject; }


			int Width()const { return w; }
			int Height()const { return h; }
			int Depth()const { return d; }

			TextureTarget GetTextureTarget()const { return target; }
			TextureFormat GetTextureFormat()const { return format; }

			unsigned int Handle()const { return handle; }


			//void CreatePixelBufferObject(int w,int h,int d) = delete;
			//void DestroyPixelBufferObject() = delete;

			void SetSubTextureData(void * data,ImageFormat imageFormat,ImageType imageType,int offset, int yoffset, int zoffset, int w, int h, int d);

			//void * MapSubTexture(int xoffset, int yoffset, int zoffset, int w, int h, int d) = delete;
			//void UnmappedSubTexture() = delete;


		private:

			// std::unique_ptr<BufferObject> pixelBufferObject; this is used to swap data between texture and memory
			Ref<BufferObject> bufferObject;
			Ref<TexCreateParams> createParams = nullptr;
			Ref<TexParams> texParams = nullptr;

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

		Ref<Texture> VISUALMAN_EXPORT_IMPORT  MakeVolumeTexture(const std::string & fileName, size_t x, size_t y, size_t z);
		Ref<Texture> VISUALMAN_EXPORT_IMPORT  MakeImageTexuture(const std::string & fileName);
		Ref<Texture> VISUALMAN_EXPORT_IMPORT  MakeTransferFunction1DTexture(const std::string & fileName);
		Ref<Texture> VISUALMAN_EXPORT_IMPORT  MakeTransferFunction1DTexture(const std::vector<Color> & colors);
	}
}


#endif

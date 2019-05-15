
#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "graphictype.h"
#include "bufferobject.h"

namespace ysl
{
	namespace graphics
	{

		class TexParams
		{
		public:
			TexParams()=default;
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
			TexFilter minFilter;
			TexFilter maxFilter;
			TexWrap wrapS;
			TexWrap wrapT;
			TexWrap wrapR;

		};

		class TexCreateParams
		{
		public:
			TexCreateParams()
			{
				
			}
		private:

			// Default parameters
			TextureDim texDim = TD_TEXTURE_2D;
			TextureFormat texFormat = TF_RGBA;
			bool border = false;
			bool mipMap = false;
			int nSamples = 0;
			Size3 texSize{0,0,0};
		};


		class GRAPHICS_EXPORT_IMPORT Texture 
		{
		public:
			Texture() = default;
			~Texture();
			void Create();
			void Destroy();

			const TexParams &TextureParameters()const { return texParams; };
			Ref<BufferObject> GetBufferObject() { return bufferObject; }
			Ref<const BufferObject> GetBufferObject() const { return bufferObject; }

			
		private:
			Ref<BufferObject> bufferObject;
			TexCreateParams createParams;
			TexParams texParams;
			TextureFormat texFormat;
			TextureDim texDim;
			Size3 texSize{0,0,0};
			int samples;
			unsigned int handle;
			
		};
	}
}


#endif

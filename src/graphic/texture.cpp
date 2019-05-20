
#include "texture.h"
#include "GL/gl3w.h"
#include "../opengl/openglutils.h"

namespace ysl
{
	namespace vpl
	{
		int GetBaseFormatBySizedFormat(TextureFormat sizedFormat)
		{
			switch (sizedFormat)
			{


			case TF_RED:
			case TF_R8:
			case TF_R8_SNORM:
			case TF_R16:
			case TF_R16_SNORM:
			case TF_R16F:
			case TF_R32F:
				return GL_RED;

			case TF_RG:
			case TF_RG8:
			case TF_RG8_SNORM:
			case TF_RG16:
			case TF_RG16_SNORM:
			case TF_RG16F:
			case TF_RG32F:
				return GL_RG;

			case TF_RGB:
			case TF_RGB4:
			case TF_RGB5:
			case TF_RGB8:
			case TF_RGB8_SNORM:
			case TF_RGB10:
			case TF_RGB12:
			case TF_RGB16:
				// case TF_RGB16_SNORM:
			case TF_RGB16F:
			case TF_RGB32F:
			case TF_R3_G3_B2:
			case TF_R11F_G11F_B10F:
				// case TF_RGB9_A5:
			case TF_SRGB8:
				return GL_RGB;

			case TF_RGBA:
			case TF_RGBA2:
			case TF_RGBA4:
			case TF_RGBA8:
			case TF_RGBA8_SNORM:
			case TF_RGBA12:
			case TF_RGBA16:
			case TF_RGBA16_SNORM:
			case TF_RGBA16F:
			case TF_RGBA32F:
			case TF_RGB5_A1:
			case TF_RGB10_A2:
			case TF_SRGB8_ALPHA8:
				return GL_RGBA;

			case TF_R8I:
			case TF_R8UI:
			case TF_R16I:
			case TF_R16UI:
			case TF_R32I:
			case TF_R32UI:
				return GL_RED_INTEGER;

			case TF_RG8I:
			case TF_RG8UI:
			case TF_RG16I:
			case TF_RG16UI:
			case TF_RG32I:
			case TF_RG32UI:
				return GL_RG_INTEGER;

			case TF_RGB8I:
				// case TF_RGB8I_EXT: // Has the same value has above
			case TF_RGB8UI:
				// case TF_RGB8UI_EXT: // Has the same value has above
			case TF_RGB16I:
				// case TF_RGB16I_EXT: // Has the same value has above
			case TF_RGB16UI:
				// case TF_RGB16UI_EXT: // Has the same value has above
			case TF_RGB32I:
				// case TF_RGB32I_EXT: // Has the same value has above
			case TF_RGB32UI:
				// case TF_RGB32UI_EXT: // Has the same value has above
				return GL_RGB_INTEGER;

			case TF_RGBA8I:
				// case TF_RGBAI_EXT: // Has the same value has above
			case TF_RGBA8UI:
				// case TF_RGBA8UI_EXT: // Has the same value has above
			case TF_RGBA16I:
				// case TF_RGBA16I_EXT: // Has the same value has above
			case TF_RGBA16UI:
				// case TF_RGBA16UI_EXT: // Has the same value has above
			case TF_RGBA32I:
				// case TF_RGBA32I_EXT: // Has the same value has above
			case TF_RGBA32UI:
				// case TF_RGBA32UI_EXT: // Has the same value has above
			case TF_RGB10_A2UI:
				return GL_RGBA_INTEGER;

			case TF_DEPTH_STENCIL:
			case TF_DEPTH24_STENCIL8:
			case TF_DEPTH32F_STENCIL8:
				return GL_DEPTH_STENCIL;

			case TF_DEPTH_COMPONENT:
			case TF_DEPTH_COMPONENT16:
			case TF_DEPTH_COMPONENT24:
			case TF_DEPTH_COMPONENT32:
			case TF_DEPTH_COMPONENT32F:
				return GL_DEPTH_COMPONENT;



			default:
				return GL_RED;
			}
		}

		int GetTypeBySizedFormat(TextureFormat sizedFormat)
		{
			switch (sizedFormat)
			{
			case TF_R8:
			case TF_R8UI:
			case TF_RG8:
			case TF_RG8UI:
			case TF_RGB8:
			case TF_RGB8UI:
			case TF_RGBA8:
			case TF_RGBA8UI:
				return GL_UNSIGNED_BYTE;
			case TF_R8I:
			case TF_RG8I:
			case TF_RGB8I:
			case TF_RGBA8I:
				return GL_BYTE;
			case TF_R16:
			case TF_R16UI:
			case TF_RG16:
			case TF_RG16UI:
			case TF_RGB10:
			case TF_RGB12:
			case TF_RGB16:
			case TF_RGB16UI:
			case TF_RGBA12:
			case TF_RGBA16:
			case TF_RGBA16UI:
			case TF_DEPTH_COMPONENT16:
				return GL_UNSIGNED_SHORT;


			case TF_R16I:
			case TF_RG16I:
			case TF_RGB16I:
			case TF_RGBA16I:
				return GL_SHORT;


			case TF_R32UI:
			case TF_RG32UI:
			case TF_RGB32UI:
			case TF_RGBA32UI:
			case TF_DEPTH_COMPONENT24:
			case TF_DEPTH_COMPONENT32:
				return GL_UNSIGNED_INT;


			case TF_R32I:
			case TF_RG32I:
			case TF_RGB32I:
			case TF_RGBA32I:
				return GL_INT;

			case TF_DEPTH24_STENCIL8:
				return GL_UNSIGNED_INT_24_8;

			case TF_R16F:
			case TF_R32F:
			case TF_RG16F:
			case TF_RG32F:
			case TF_RGB16F:
			case TF_RGB32F:
			case TF_RGBA16F:
			case TF_RGBA32F:
			case TF_R11F_G11F_B10F:
			case TF_DEPTH_COMPONENT32F:
				return GL_FLOAT;

			case TF_DEPTH32F_STENCIL8:
				return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;

			default:
				return GL_UNSIGNED_BYTE;
			}
		}

		Texture::~Texture()
		{
			DestroyTexture();
		}

		/**
		 * \brief  Create a texture by given \a TexCreateParams
		 */
		bool Texture::CreateTexture()
		{
			if (!createParams)
			{
				Debug("Setup Parameters have not been set.\n");
				return false;
			}

			return CreateTexture(createParams->GetTextureTarget(),
				createParams->GetTextureFormat(),
				createParams->Width(),
				createParams->Height(),
				createParams->Depth(),
				createParams->IsBorder(),
				createParams->GetBufferObject(),
				0);

		}

		/**
		 * \brief Create a texture by the given parameters
		 *
		 *  A buffer object is need only when the target is a buffer texture.
		 */
		bool Texture::CreateTexture(TextureTarget target,
			TextureFormat texFormat,
			int w,
			int h,
			int d,
			bool border,
			Ref<BufferObject> bufferObject,
			int samples)
		{
			DestroyTexture();

			GL(glCreateTextures(target, 1, &handle));
			if(handle == 0)
			{
				Warning("Texture can not be created. %s %d",__FILE__,__LINE__);
				return false;
			}

			if (target == TD_TEXTURE_1D)
			{
				GL(glTextureStorage1D(handle, 1, texFormat, w));
				GL(glTextureParameteri(handle, GL_TEXTURE_WRAP_R, texParams->WrapR()));
				GL(glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, texParams->MaxFilter()));
				GL(glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, texParams->MinFilter()));
				//createParams.reset();
			}
			else if (target == TD_TEXTURE_2D || target == TD_TEXTURE_RECTANGLE)
			{
				GL(glTextureStorage2D(handle, 1, texFormat, w, h));
				GL(glTextureParameteri(handle, GL_TEXTURE_WRAP_R, texParams->WrapR()));
				GL(glTextureParameteri(handle, GL_TEXTURE_WRAP_S, texParams->WrapS()));
				GL(glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, texParams->MaxFilter()));
				GL(glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, texParams->MinFilter()));
				//createParams.reset();
			}
			else if (target == TD_TEXTURE_3D)
			{
				GL(glTextureStorage3D(handle, 1, texFormat, w, h, d));
				GL(glTextureParameteri(handle, GL_TEXTURE_WRAP_R, texParams->WrapR()));
				GL(glTextureParameteri(handle, GL_TEXTURE_WRAP_S, texParams->WrapS()));
				GL(glTextureParameteri(handle, GL_TEXTURE_WRAP_T, texParams->WrapT()));
				GL(glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, texParams->MaxFilter()));
				GL(glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, texParams->MinFilter()));

				//createParams.reset();
			}
			else
			{
				assert(false);
				Debug("Unsupported format");
			}
			// Set up the texture properties and delete the setup params 

			this->target = target;
			this->format = texFormat;
			this->d = d;
			this->w = w;
			this->h = h;
			this->samples = samples;
			this->border = border;


			return true;
		}

		void Texture::DestroyTexture()
		{
			if (handle)
			{
				GL(glDeleteTextures(1, &handle));
				handle = 0;
			}
		}

		void Texture::SetSubTextureData(void* data, ImageFormat imageFormat, ImageType imageType, int xoffset, int yoffset, int zoffset, int w, int h, int d)
		{
			if (handle == 0)
			{
				if (!CreateTexture())
				{
					Debug("Texture has not beed created. But it created failed\n. %s %d", __FILE__, __LINE__);
					return;
				}
			}

			if (target == GL_TEXTURE_1D)
			{
				GL(glTextureSubImage1D(handle, 0, xoffset, w, imageFormat, imageType, data));
			}
			else if (target == GL_TEXTURE_2D || target == GL_TEXTURE_RECTANGLE)
			{
				GL(glTextureSubImage2D(handle, 0, xoffset, yoffset, w, h, imageFormat, imageType, data));

			}
			else if (target == GL_TEXTURE_3D)
			{
				GL(glTextureSubImage3D(handle, 0, xoffset, yoffset, zoffset, w, h, d, imageFormat, imageType, data));
			}
			else
			{
				Debug("Unsuported format. %s %d", __FILE__, __LINE__);
			}
		}
	}
}

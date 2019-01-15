#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "../../lib/gl3w/GL/glcorearb.h"
#include "transformation.h"
#include "openglcontext.h"
#include "../volume/cachepolicy.h"

//#include "openglutils.h"

#include "../utility/utils.h"

class OpenGLTexture
{
public:
	enum TextureUnit
	{
		TextureUnit0 = GL_TEXTURE0,
		TextureUnit1 = GL_TEXTURE1,
		TextureUnit2 = GL_TEXTURE2,
		TextureUnit3 = GL_TEXTURE3,
		TextureUnit4 = GL_TEXTURE4,
		TextureUnit5 = GL_TEXTURE5,
		TextureUnit6 = GL_TEXTURE6
	};
	enum TextureTarget
	{
		Texture1D = GL_TEXTURE_1D,
		Texture2D = GL_TEXTURE_2D,
		Texture3D = GL_TEXTURE_3D,
		Texture2DRect = GL_TEXTURE_RECTANGLE
	};
	enum WrapMode
	{
		Repeat = GL_REPEAT,
		MirrorRepeat = GL_MIRRORED_REPEAT,
		ClampToEdge = GL_CLAMP_TO_EDGE,
		ClampToBorder = GL_CLAMP_TO_BORDER
	};
	enum FilterMode
	{
		Nearest = GL_NEAREST,
		Linear = GL_LINEAR
	};
	enum InternalFormat
	{
		R8 = GL_R8,
		R16F = GL_R16F,
		RGB8 = GL_RGB8,
		RGBA32F = GL_RGBA32F,
		RGBA32UI = GL_RGBA32UI,
		InternalDepthComponent = GL_DEPTH_COMPONENT
	};

	enum  ExternalDataFormat
	{
		RED = GL_RED,
		RGB = GL_RGB,
		RGBA = GL_RGBA,
		RGBAInteger = GL_RGBA_INTEGER,
		ExternalDepthComponent = GL_DEPTH_COMPONENT
		
	};




	enum Access
	{
		Read= GL_READ_ONLY,
		Write = GL_WRITE_ONLY,
		ReadAndWrite = GL_READ_WRITE
	};

	
	enum ExternalDataType
	{
		UInt32 = GL_UNSIGNED_INT,
		UInt8 = GL_UNSIGNED_BYTE,
		Float32 = GL_FLOAT
	};


	OpenGLTexture(TextureTarget target,
		FilterMode min = FilterMode::Linear,
		FilterMode mag = FilterMode::Linear,
		WrapMode rWrapMode = WrapMode::ClampToEdge,
		WrapMode sWrapMode = WrapMode::ClampToEdge,
		WrapMode tWrapMode = WrapMode::ClampToEdge);

	DISABLE_COPY(OpenGLTexture);

	DISABLE_MOVE(OpenGLTexture);

	void Bind();
	void Unbind();
	void SetFilterMode(FilterMode minMode = Linear, FilterMode magMode= Linear);
	void SetWrapMode(WrapMode rMode,WrapMode sMode = ClampToEdge,WrapMode tMode = ClampToEdge);
	void SetData(InternalFormat internalFmt,ExternalDataFormat extFmt,ExternalDataType extType,int x,int y,int z,void * data);
	void SetSubData(
		ExternalDataFormat extFmt, 
		ExternalDataType extType,int xOffset, int x, int yOffset,int y, int zOffset,int z, const void * data);
	TextureTarget Target()const;

	void GetTexture(int level,
	                int xOffset, int yOffset, int zOffset,
	                std::size_t width, std::size_t height, std::size_t depth,
	                ExternalDataFormat fmt, ExternalDataType type, std::size_t bufSize,
	                void* pixels);
	void BindToDataImage(int imageUnit,int level,bool layered,int layer,Access access,InternalFormat fmt);
	void SaveAsImage(const std::string & fileName);
	unsigned int NativeTextureId()const;
	~OpenGLTexture();

	static std::shared_ptr<OpenGLTexture> CreateTexture1D(InternalFormat fmt,
		FilterMode min,FilterMode mag,WrapMode rWrapMode,
		ExternalDataFormat extFmt,
		ExternalDataType type,
		int width,
		void * data);

	static std::shared_ptr<OpenGLTexture> CreateTexture2D(InternalFormat fmt,
		FilterMode min, FilterMode mag, WrapMode rWrapMode,WrapMode sWrapMode,
		ExternalDataFormat extFmt,
		ExternalDataType type,
		int width,int height,void * data);

	static std::shared_ptr<OpenGLTexture> CreateTexture2DRect(InternalFormat fmt,
		FilterMode min, FilterMode mag, WrapMode rWrapMode, WrapMode sWrapMode,
		ExternalDataFormat extFmt,
		ExternalDataType type,int width,int height,void * data);

	static std::shared_ptr<OpenGLTexture> CreateTexture3D(InternalFormat fmt,
		FilterMode min, FilterMode mag, WrapMode rWrapMode, WrapMode sWrapMode,WrapMode tWrapMode,
		ExternalDataFormat extFmt,
		ExternalDataType type,int width,int height,int depth,void * data);

private:
	unsigned int textureId;
	std::shared_ptr<OpenGLCurrentContext> currentContext;
	TextureTarget target;
};



#endif

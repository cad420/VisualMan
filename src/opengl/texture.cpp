
#include "../../lib/gl3w/GL/gl3w.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../lib/3rdparty/stb_image_write.h"

#include "texture.h"
#include "openglutils.h"
#include "error.h"
#include "openglcontext.h"

OpenGLTexture::OpenGLTexture(TextureTarget target_, 
	FilterMode min, 
	FilterMode mag,
	WrapMode rWrapMode,
	WrapMode sWrapMode, 
	WrapMode tWrapMode):target(target_)
{
	currentContext = OpenGLCurrentContext::GetCurrentOpenGLContext();
	if (!currentContext->IsValid())
		ysl::Error("No OpenGL Context.");
	glGenTextures(1, &textureId);
	glBindTexture(target, textureId);
	if(target_ != TextureBuffer)
	{
		SetFilterMode(min, mag);
		SetWrapMode(rWrapMode, sWrapMode, tWrapMode);
	}
}

void OpenGLTexture::Bind()
{
	glBindTexture(target, textureId);
}

void OpenGLTexture::Unbind()
{
	glBindTexture(target, 0);
}

void OpenGLTexture::SetFilterMode(FilterMode minMode, FilterMode magMode)
{
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minMode);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magMode);
}

void OpenGLTexture::SetWrapMode(WrapMode rMode, WrapMode sMode, WrapMode tMode)
{	
	glTexParameteri(target, GL_TEXTURE_WRAP_R, rMode);
	if(target == Texture2D || target== Texture2DRect || target == Texture3D)
		glTexParameteri(target, GL_TEXTURE_WRAP_S, sMode);
	if(target== Texture3D)
		glTexParameteri(target, GL_TEXTURE_WRAP_T, tMode);
}

void OpenGLTexture::SetData(InternalFormat internalFmt,
	ExternalDataFormat extFmt, 
	ExternalDataType extType, int x,int y,int z,void * data)
{
	Bind();
	if (target == Texture1D )
		glTexImage1D(target, 0, internalFmt, x, 0, extFmt, extType, data);
	if (target == Texture2D || target == Texture2DRect)
		glTexImage2D(target, 0, internalFmt, x, y, 0, extFmt, extType, data);
	if (target == Texture3D)
		glTexImage3D(target, 0, internalFmt, x, y, z, 0, extFmt, extType, data);
}

void OpenGLTexture::SetSubData(
	ExternalDataFormat extFmt, 
	ExternalDataType extType, int xOffset,int x,
	int yOffset,int y, int zOffset, int z,const void* data)
{
	Bind();
	if (target == Texture1D )
		glTexSubImage1D(target, 0, xOffset, x,  extFmt, extType, data);
	if (target == Texture2D || target == Texture2DRect)
		glTexSubImage2D(target, 0, xOffset,yOffset, x, y,  extFmt, extType, data);
	if (target == Texture3D)
		glTexSubImage3D(target, 0, xOffset,yOffset,zOffset,x, y, z, extFmt, extType, data);
}

OpenGLTexture::TextureTarget OpenGLTexture::Target() const
{
	return target;
}

void OpenGLTexture::GetTexture(int level,
                               int xOffset,
                               int yOffset,
                               int zOffset,
                               std::size_t width,
                               std::size_t height,
                               std::size_t depth,
                               OpenGLTexture::ExternalDataFormat fmt,
                               OpenGLTexture::ExternalDataType type,
                               std::size_t bufSize, void* pixels)
{
	glGetTextureSubImage(textureId, 
		level, xOffset, yOffset,zOffset, 
		width, height, depth, 
		fmt, type, bufSize, pixels);
}

void OpenGLTexture::BindToDataImage(int imageUnit, int level, bool layered, int layer, Access access,
	InternalFormat fmt)
{
	Bind();
	glBindImageTexture(imageUnit, textureId, level, layered, layer, access, fmt);
}

void OpenGLTexture::BindTextureBuffer(std::shared_ptr<OpenGLBuffer> buffer, OpenGLTexture::InternalFormat format)
{
	if(buffer->Target() == OpenGLBuffer::TextureBuffer)
	{
		Bind();
		glTexBuffer(GL_TEXTURE_BUFFER, format,buffer->bufferId);
		GL_ERROR_REPORT;
	}
}

void OpenGLTexture::SaveAsImage(const std::string& fileName)
{
	Bind();
	int texDim[2];
	glGetTexLevelParameteriv(target, 0, GL_TEXTURE_WIDTH, texDim);
	glGetTexLevelParameteriv(target, 0, GL_TEXTURE_HEIGHT, texDim+1);
	GL_ERROR_REPORT;
	std::unique_ptr<char[]> imageBuf(new char[std::size_t(texDim[0])*texDim[1] * 4 * sizeof(char)]);
	glGetTexImage(target, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageBuf.get());
	GL_ERROR_REPORT;
	stbi_write_jpg(fileName.c_str(), texDim[0], texDim[1], 4, imageBuf.get(), 100);

}

unsigned OpenGLTexture::NativeTextureId()const
{
	return textureId;
}

OpenGLTexture::~OpenGLTexture()
{
	if (*currentContext != *OpenGLCurrentContext::GetCurrentOpenGLContext())
		ysl::Warning("Delete the resource that created in another context.");
	glDeleteTextures(1, &textureId);
}

std::shared_ptr<OpenGLTexture> OpenGLTexture::CreateTexture1D(InternalFormat fmt,
	FilterMode min, 
	FilterMode mag,
	WrapMode rWrapMode, 
	ExternalDataFormat extFmt,
	ExternalDataType type, 
	int width, 
	void* data)
{
	auto ptr = std::make_shared<OpenGLTexture>(Texture1D,
		min,
		mag,
		rWrapMode,
		ClampToEdge,
		ClampToEdge);
	if(ptr)
		ptr->SetData(fmt, extFmt, type, width, 0, 0, data);
	return ptr;
}

std::shared_ptr<OpenGLTexture> OpenGLTexture::CreateTexture2D(InternalFormat fmt, 
	FilterMode min,
	FilterMode mag,
	WrapMode rWrapMode,
	WrapMode sWrapMode,
	ExternalDataFormat extFmt,
	ExternalDataType type,
	int width, 
	int height,
	void* data)
{

	auto ptr = std::make_shared<OpenGLTexture>(Texture2D,
		min,
		mag,
		rWrapMode,
		sWrapMode,
		ClampToEdge);
	if (ptr)
		ptr->SetData(fmt, extFmt, type, width, height, 0, data);
	return ptr;
}

std::shared_ptr<OpenGLTexture> OpenGLTexture::CreateTexture2DRect(InternalFormat fmt, 
	FilterMode min,
	FilterMode mag, 
	WrapMode rWrapMode, 
	WrapMode sWrapMode, 
	ExternalDataFormat extFmt, 
	ExternalDataType type, 
	int width, 
	int height, 
	void * data)
{
	auto ptr = std::make_shared<OpenGLTexture>(Texture2DRect,min,mag,rWrapMode,sWrapMode,ClampToEdge);
	if (ptr)
		ptr->SetData(fmt, extFmt, type, width, height, 0, data);
	return ptr;
}

std::shared_ptr<OpenGLTexture> OpenGLTexture::CreateTexture3D(InternalFormat fmt,
	FilterMode min,
	FilterMode mag,
	WrapMode rWrapMode,
	WrapMode sWrapMode,
	WrapMode tWrapMode,
	ExternalDataFormat extFmt,
	ExternalDataType type,
	int width,
	int height,
	int depth,
	void * data)
{
	auto ptr = std::make_shared<OpenGLTexture>(Texture3D, min, mag, rWrapMode, sWrapMode, tWrapMode);
	if (ptr)
	{
		ptr->Bind();
		ptr->SetData(fmt, extFmt, type, width, height, depth, data);
	}
		
	return ptr;
}

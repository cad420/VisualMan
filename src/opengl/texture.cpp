
#include "texture.h"

#include "../../lib/gl3w/GL/gl3w.h"
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
	SetFilterMode(min, mag);
	SetWrapMode(rWrapMode, sWrapMode, tWrapMode);
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
	if (target == Texture1D)
		glTexImage1D(target, 0, internalFmt, x, 0, extFmt, extType, data);
	if (target == Texture2D || target == Texture2DRect)
		glTexImage2D(target, 0, internalFmt, x, y, 0, extFmt, extType, data);
	if (target == Texture3D)
		glTexImage3D(target, 0, internalFmt, x, y, z, 0, extFmt, extType, data);
}

OpenGLTexture::TextureTarget OpenGLTexture::Target() const
{
	return target;
}

void OpenGLTexture::BindToDataImage(int imageUnit, int level, bool layered, int layer, Access access,
	InternalFormat fmt)
{
	Bind();
	glBindImageTexture(imageUnit, textureId, level, layered, layer, access, fmt);
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
		ptr->SetData(fmt, extFmt, type, width, height, depth, data);
	return ptr;
}

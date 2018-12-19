
#include "texture.h"

#include "../../lib/gl3w/GL/gl3w.h"

OpenGLTexture::OpenGLTexture(TextureTarget target, 
	InternalFormat intFmt,
	FilterMode min, 
	FilterMode mag, 
	WrapMode wrap):
m_textureId(0),
m_target(target),
m_wrapMode(wrap),
m_minFilterMode(min),
m_magFilterMode(mag),
m_intFmt(intFmt)
{
}

void OpenGLTexture::create()
{
	glGenTextures(1, &m_textureId);
}

void OpenGLTexture::setMagnificationFilter(FilterMode mode)
{
	m_magFilterMode = mode;
}

void OpenGLTexture::setMinificationFilter(FilterMode mode)
{
	m_minFilterMode = mode;
}

void OpenGLTexture::setWrapMode(WrapMode mode)
{
	m_wrapMode = mode;
}

void OpenGLTexture::setInternalFormat(InternalFormat fmt)
{
	m_intFmt = fmt;
}

void OpenGLTexture::setSize(int x, int y, int z)
{
	m_xSize = x;
	m_ySize = y;
	m_zSize = z;
}

unsigned OpenGLTexture::textureId() const
{
	return m_textureId;
}


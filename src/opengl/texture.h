#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "../../lib/gl3w/GL/glcorearb.h"

//#include "openglutils.h"

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
	R16 = GL_R16,
	RGB8 = GL_RGB8
};

class OpenGLTexture
{
public:


	enum class ExternalDataFormat{};
	enum class ExternalDataType{};

	OpenGLTexture(TextureTarget target, 
		InternalFormat intFmt,
		FilterMode min = FilterMode::Linear, 
		FilterMode mag = FilterMode::Linear, 
		WrapMode wrap = WrapMode::ClampToEdge);

	void create();
	void setMagnificationFilter(FilterMode mode);
	void setMinificationFilter(FilterMode mode);
	void setWrapMode(WrapMode mode);
	void setInternalFormat(InternalFormat fmt);
	void setSize(int x, int y, int z);
	void allocate();
	void setData(ExternalDataFormat extFmt,ExternalDataType extType,void * data);
	unsigned int textureId()const;

	~OpenGLTexture() = default;

private:
	int m_xSize, m_ySize, m_zSize;
	unsigned int m_textureId;
	TextureTarget m_target;
	WrapMode m_wrapMode;
	FilterMode m_minFilterMode;
	FilterMode m_magFilterMode;
	InternalFormat m_intFmt;

};

class OpenGLImage
{
public:
	enum class TextureTarget { Texture1D, Texture2D, Texture3D, Texture2DRect };
	enum class WrapMode { Repeat, MirrorRepeat, ClampToEdge, ClampToBorder };
	enum class FilterMode { Nearest, Linear };
	enum class InternalFormat {};
	enum class ExternalFormat {};

	OpenGLImage(TextureTarget target,
		InternalFormat intFmt,
		FilterMode min = FilterMode::Linear,
		FilterMode mag = FilterMode::Linear,
		WrapMode wrap = WrapMode::ClampToEdge);

	void create();
	void setMagnificationFilter(FilterMode mode);
	void setMinificationFilter(FilterMode mode);
	void setWrapMode(WrapMode mode);
	void setInternalFormat(InternalFormat fmt);
	void setSize(int x, int y, int z);
	unsigned int textureId()const;

private:
	int m_xSize, m_ySize, m_zSize;
	unsigned int m_textureId;
	TextureTarget m_target;
	WrapMode m_wrapMode;
	FilterMode m_minFilterMode;
	FilterMode m_magFilterMode;
	InternalFormat m_intFmt;
};

#endif


#ifndef _TEXTURE2D_H_
#define _TEXTURE2D_H_

namespace ysl
{
	class Texture2D
	{
	public:
		Texture2D();
		virtual ~Texture2D() = 0;
	};


	class OpenGLTexture2D :public Texture2D
	{
	public:

	};
}

#endif
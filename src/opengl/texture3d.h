
#ifndef _TEXTURE3D_H_
#define _TEXTUre3D_h_

namespace ysl
{

	class Texture3D
	{
	public:
		Texture3D();
		virtual ~Texture3D() = 0;
	};

	class OpenGLTexture3D :public Texture3D
	{
	public:

	};
}


#endif
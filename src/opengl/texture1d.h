

#ifndef _TEXTURE1D_H_
#define _TEXTURE1D_H_
#include "openglobject.h"

namespace ysl
{
	class Texture1D:public GPUObject
	{
	public:
		Texture1D();
		virtual ~Texture1D() = 0;
	};



	class OpenGLTexture1D:public Texture1D
	{
		public:

	};


}

#endif
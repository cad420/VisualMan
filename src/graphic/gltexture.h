
#ifndef _GLTEXTURE_H_
#define _GLTEXTURE_H_

#include "globject.h"

namespace ysl
{
	namespace gpu
	{
		namespace gl
		{
			class GLTexture:public GLObject
			{
			public:
				GLTexture() = default;
				virtual ~GLTexture() = default;
			};
		}
	}
}


#endif

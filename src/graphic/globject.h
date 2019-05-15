
#ifndef _GLOBJECT_H_
#define _GLOBJECY_H_
#include "../gpuobject/gpuobject.h"

namespace ysl
{
	namespace graphics
	{
		namespace gl
		{
			class GLObject:public GPUObject
			{
			public:
				GLObject() = default;
				virtual ~GLObject() = default;
			};
		}
	}
}

#endif
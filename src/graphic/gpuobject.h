
#ifndef _GPUOBJECT_H_
#define _GPUOBJECT_H_
#include <cstdint>
#include "../system/object.h"


namespace ysl
{
	namespace graphics
	{
		class GPUObject:public Object
		{
			DECLARE_RTTI
			DECLARE_INITIAL(GPUObject)
			static uint64_t memoryUsage;
		public:
			GPUObject();
			virtual ~GPUObject();
		};

		IMPLEMENT_RTTI_NoConstructor(GPUObject,Object)


	}
}


#endif

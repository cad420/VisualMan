
#include "renderstate.h"
#include <iostream>

namespace ysl
{
	namespace gpu
	{
		void DepthFuncState::Apply(int index, Ref<Camera> camera, RenderContext* context) const
		{
			std::cout << "Apply Depth Function State\n";

		}
	}
}

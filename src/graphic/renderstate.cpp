
#include "renderstate.h"
#include <iostream>

namespace ysl
{
	namespace graphics
	{
		void DepthFuncState::Apply(int index, const Camera * camera, RenderContext* context) const
		{
			std::cout << "Apply Depth Function State\n";

		}

		void TextureSampler::Apply(int index, const Camera * camera, RenderContext* context) const
		{
			std::cout << "Apply TExtureSampler State\n";

		}
	}
}

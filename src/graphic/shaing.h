
#ifndef _SHADING_H_
#define _SHADING_H_
#include "abstrarenderstate.h"
#include "uniform.h"

namespace ysl
{
	namespace graphics
	{
		class Shading
		{
		public:
			Shading() = default;
		private:
			Ref<RenderStateSet> renderStateSet; // shading program is in it
			Ref <UniformSet> uniformSet;
			// Ref<EnableSet> enableSet;
		};
	}
}

#endif

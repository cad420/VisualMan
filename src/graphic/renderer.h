
#ifndef _RENDERER_H_
#define _RENDERER_H_
#include "abstrarenderer.h"
#include "renderqueue.h"

namespace ysl
{
	namespace graphics
	{



		class GRAPHICS_EXPORT_IMPORT Renderer:public AbstraRenderer
		{
		public:
			Renderer() = default;
			~Renderer() = default;
			void Render(const RenderQueue & rederQueue, const Ref<Camera> & camera)override;
		};

	}
}

#endif
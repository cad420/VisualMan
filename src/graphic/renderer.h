
#ifndef _RENDERER_H_
#define _RENDERER_H_
#include "abstrarenderer.h"

namespace ysl
{
	namespace gpu
	{
		class GRAPHICS_EXPORT_IMPORT Renderer:public AbstraRenderer
		{
		public:
			Renderer() = default;
			~Renderer() = default;
			void Render(const std::vector<Ref<Actor>> &rederQueue, const Ref<Camera> & camera)override;
			Ref<Framebuffer> GetFramebuffer()const;
			void SetFramebuffer(Ref<Framebuffer> fb) { this->framebuffer = fb; }
		protected:
			Ref<Framebuffer> framebuffer;
		};

	}
}

#endif

#ifndef _RENDERER_H_
#define _RENDERER_H_
#include "abstrarenderer.h"
#include "renderqueue.h"

namespace ysl
{
	namespace graphics
	{

		class Framebuffer;

		class GRAPHICS_EXPORT_IMPORT Renderer:public AbstraRenderer
		{
		public:
			Ref<Framebuffer> GetFramebuffer() override { return framebuffer; }
			void SetFramebuffer(Ref<Framebuffer> fb) { framebuffer = std::move(fb); }
			void Render(const RenderQueue & rederQueue, const Ref<Camera> & camera)override;
		private:
			Ref<Framebuffer> framebuffer;
		};

	}
}

#endif
#ifndef _ABSTRARENDERER_H_
#define _ABSTRARENDERER_H_

#include <vector>
#include "graphictype.h"
#include <memory>
#include "renderqueue.h"
#include "camera.h"
#include "framebuffer.h"

namespace ysl
{
	namespace graphics
	{

		class IRenderEvent;

		class GRAPHICS_EXPORT_IMPORT AbstraRenderer
		{
		public:
			AbstraRenderer() = default;
			virtual ~AbstraRenderer() = default;
			virtual void Render(const RenderQueue & rederQueue, const Ref<Camera> & camera) = 0;
			Ref<Framebuffer> GetFramebuffer() { return framebuffer; }
			void SetFramebuffer(Ref<Framebuffer> fb) { this->framebuffer = fb; }
			void DispatchOnRenderStartedEvent();
			void DispatchOnRenderFinishedEvent();
			void AddRenderEventCallback(Ref<IRenderEvent> callback);
		private:
			Ref<Framebuffer> framebuffer;
			std::vector<Ref<IRenderEvent>> callbacks;
		};

	}
}

#endif
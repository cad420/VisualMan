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
	namespace vm
	{

		class IRenderEvent;

		class GRAPHICS_EXPORT_IMPORT AbstraRenderer
		{
		public:
			AbstraRenderer() = default;
			virtual ~AbstraRenderer() = default;
			virtual void Render(const RenderQueue & rederQueue, const Ref<Camera> & camera) = 0;
			virtual Ref<Framebuffer> GetFramebuffer() = 0;
			void DispatchOnRenderStartedEvent();
			void DispatchOnRenderFinishedEvent();
			void AddRenderStartedEventCallback(Ref<IRenderEvent> callback);
			void AddRenderFinishedEventCallback(Ref<IRenderEvent> callback);
		private:
			std::vector<Ref<IRenderEvent>> startedCallbacks;
			std::vector<Ref<IRenderEvent>> finishedCallbacks;
		};

	}
}

#endif
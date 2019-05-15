
#include "abstrarenderer.h"
#include "renderevent.h"

namespace ysl
{
	namespace graphics
	{
		void AbstraRenderer::DispatchOnRenderStartedEvent()
		{
			std::cout << "AbstraRenderer::DispatchOnRenderStartedEvent\n";
			for(auto it = callbacks.begin();it != callbacks.end();++it)
			{
				auto & item = *it;
				if (item->IsEnabled() && item->OnRendererStartedEvent(this) && item->IsRemoveAfterCallEnabled())
					callbacks.erase(it);
			}
		}

		void AbstraRenderer::DispatchOnRenderFinishedEvent()
		{
			std::cout << "AbstraRenderer::DispatchOnRenderFinishedEent\n";
			for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
			{
				auto & item = *it;
				if (item->IsEnabled() && item->OnRendererFinishedEvent(this) && item->IsRemoveAfterCallEnabled())
					callbacks.erase(it);
			}
		}

		void AbstraRenderer::AddRenderEventCallback(Ref<IRenderEvent> callback)
		{
			callbacks.push_back(std::move(callback));
		}
	}
}
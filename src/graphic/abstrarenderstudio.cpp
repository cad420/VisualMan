
#include "abstrarenderstudio.h"
#include <iostream>
#include "../utility/error.h"

namespace ysl
{
	namespace graphics
	{
		void AbstraRenderStudio::DispatchRenderStartedEvent()
		{
			Debug("AbstraRenderStudio::DispatchRenderStartedEvent()");
			for (auto it = renderStartedcallbacks.begin(); it != renderStartedcallbacks.end(); )
			{
				auto & item = *it;
				if (item->IsEnabled() && item->OnRenderingStartedEvent(this) && item->IsRemoveAfterCallEnabled())
					it = renderStartedcallbacks.erase(it);
				else
					++it;
			}
		}

		void AbstraRenderStudio::DispatchRenderFinishedEvent()
		{
			Debug("AbstraRenderStudio::DispatchRenderFinishedEvent()");
			for (auto it = renderFinishedCallbacks.begin(); it != renderFinishedCallbacks.end(); )
			{
				auto & item = *it;
				if (item->IsEnabled() && item->OnRenderingFinishedEvent(this) && item->IsRemoveAfterCallEnabled())
					it = renderFinishedCallbacks.erase(it);
				else
					++it;
			}
		}

		void AbstraRenderStudio::AddRenderStartedEventCallback(Ref<IRenderEvent> callback)
		{
			renderStartedcallbacks.push_back(std::move(callback));
		}

		void AbstraRenderStudio::AddRenderFinishedEventCallback(Ref<IRenderEvent> callback)
		{
			renderFinishedCallbacks.push_back(std::move(callback));
		}
	}
}

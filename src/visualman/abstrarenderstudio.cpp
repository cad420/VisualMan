
#include "abstrarenderstudio.h"

namespace ysl
{
	namespace vm
	{
		void AbstraAggregate::DispatchRenderStartedEvent()
		{
			//Debug("AbstraRenderStudio::DispatchRenderStartedEvent()");
			for (auto it = renderStartedcallbacks.begin(); it != renderStartedcallbacks.end(); )
			{
				auto & item = *it;
				if (item->IsEnabled() && item->OnRenderingStartedEvent(this) && item->IsRemoveAfterCallEnabled())
					it = renderStartedcallbacks.erase(it);
				else
					++it;
			}
		}

		void AbstraAggregate::DispatchRenderFinishedEvent()
		{
			//Debug("AbstraRenderStudio::DispatchRenderFinishedEvent()");
			for (auto it = renderFinishedCallbacks.begin(); it != renderFinishedCallbacks.end(); )
			{
				auto & item = *it;
				if (item->IsEnabled() && item->OnRenderingFinishedEvent(this) && item->IsRemoveAfterCallEnabled())
					it = renderFinishedCallbacks.erase(it);
				else
					++it;
			}
		}

		void AbstraAggregate::AddRenderStartedEventCallback(Ref<IRenderEvent> callback)
		{
			renderStartedcallbacks.push_back(std::move(callback));
		}

		void AbstraAggregate::AddRenderFinishedEventCallback(Ref<IRenderEvent> callback)
		{
			renderFinishedCallbacks.push_back(std::move(callback));
		}
	}
}

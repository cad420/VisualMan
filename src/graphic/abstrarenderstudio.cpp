
#include "abstrarenderstudio.h"
#include <iostream>
#include "../utility/error.h"

namespace ysl
{
	namespace graphics
	{
		void AbstraFrame::DispatchRenderStartedEvent()
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

		void AbstraFrame::DispatchRenderFinishedEvent()
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

		void AbstraFrame::AddRenderStartedEventCallback(Ref<IRenderEvent> callback)
		{
			renderStartedcallbacks.push_back(std::move(callback));
		}

		void AbstraFrame::AddRenderFinishedEventCallback(Ref<IRenderEvent> callback)
		{
			renderFinishedCallbacks.push_back(std::move(callback));
		}
	}
}

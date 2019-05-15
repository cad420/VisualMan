
#include "abstrarenderstudio.h"
#include <iostream>

namespace ysl
{
	namespace graphics
	{
		void AbstraRenderStudio::DispatchRenderStartedEvent()
		{
			std::cout << "AbstraRenderStudio::DispatchRenderStartedEvent()" << std::endl;
			for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
			{
				auto & item = *it;
				if (item->IsEnabled() && item->OnRenderingStartedEvent(this) && item->IsRemoveAfterCallEnabled())
					callbacks.erase(it);
			}
		}

		void AbstraRenderStudio::DispatchRenderFinishedEvent()
		{
			std::cout << "AbstraRenderStudio::DispatchRenderFinishedEvent()" << std::endl;
			for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
			{
				auto & item = *it;
				if (item->IsEnabled() && item->OnRenderingFinishedEvent(this) && item->IsRemoveAfterCallEnabled())
					callbacks.erase(it);
			}
		}

		void AbstraRenderStudio::AddRenderEventCallback(Ref<IRenderEvent> callback)
		{
			callbacks.push_back(std::move(callback));
		}
	}
}


#include "actor.h"

namespace ysl
{
	namespace graphics
	{
		void Actor::DispatchOnActorRenderStartedEvent()
		{
			for(auto & each:callbacks)
			{
				if (each->IsEventEnable())
					each->OnActorRenderStartedEvent();
			}
		}

		void Actor::DispatchOnActorRenderFinishedEvent()
		{
			for (auto & each : callbacks)
			{
				if (each->IsEventEnable())
					each->OnActorRenderFinishedEvent();
			}
		}

		void Actor::AddActorEventCallback(Ref<IActorEvent> callback)
		{
			callbacks.push_back(std::move(callback));
		}
	}
}
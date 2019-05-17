
#include "actor.h"
#include "renderable.h"

namespace ysl
{
	namespace graphics
	{
		Actor::Actor(Ref<Renderable> renderable, Ref<Artist> art, Ref<Transform> transform) :
			art(std::move(art)), transform(std::move(transform))
		{
			renderables[0] = std::move(renderable);
		}

		void Actor::DispatchOnActorRenderStartedEvent(const Camera * camera, Renderable * renderable, const Artist * art, int pass)
		{
			for (auto & each : actorEvents)
			{
				if (each->IsEventEnable())
					each->OnActorRenderStartedEvent(this, camera, renderable, art, pass);
			}
		}

		void Actor::DispatchOnActorDeletingEvent(const Camera * camera, Renderable * renderable, const Artist * art, int pass)
		{
			for (auto & each : actorEvents)
			{
				each->OnActorDeletingEvent(this, camera, renderable, art, pass);
			}
		}

		void Actor::AddActorRenderEventCallback(Ref<IActorEvent> callback)
		{
			actorEvents.push_back(std::move(callback));
		}

		Ref<Renderable> Actor::GetRenderableFromLod(int lod)
		{
			assert(lod >= 0 && lod < 8);
			return renderables[lod];
		}
	}
}
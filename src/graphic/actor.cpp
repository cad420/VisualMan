
#include "actor.h"
#include "renderable.h"

namespace ysl
{
	namespace vpl
	{


		Actor::Actor(Ref<Renderable> renderable, Ref<Artist> art, Ref<Transform> transform):
			artist(std::move(art))
		{
			if (transform == nullptr)
				this->transform = MakeRef<Transform>();
			else
				this->transform = std::move(transform);
			renderables[0] = std::move(renderable);
		}

		void Actor::DispatchOnActorRenderStartedEvent(const Camera * camera, Renderable * renderable, const Shading * shading, int pass)
		{
			for (auto & each : actorEvents)
			{
				if (each->IsEventEnable())
					each->OnActorRenderStartedEvent(this, camera, renderable, shading, pass);
			}
		}

		void Actor::DispatchOnActorDeletingEvent(const Camera * camera, Renderable * renderable, const Shading * shading, int pass)
		{
			for (auto & each : actorEvents)
			{
				each->OnActorDeletingEvent(this, camera, renderable, shading, pass);
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

		int Actor::EvalLod(const Camera* camera)
		{
			if(lodEvaluator)
			{
				return lodEvaluator->Eval(this, camera);
			}
			return 0;
		}
	}
}
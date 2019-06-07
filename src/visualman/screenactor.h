
#ifndef _SCREENACTOR_H_
#define _SCREENACTOR_H_
#include "actor.h"
#include "primitive.h"

namespace ysl
{
	namespace vm
	{
		class VISUALMAN_EXPORT_IMPORT ScreenActorEventCallback:public IActorEvent
		{
		public:
			ScreenActorEventCallback();
			void OnActorRenderStartedEvent(Actor* actor, const Camera* camera, Renderable* renderable, const Shading* shading, int pass) override;
			void BindToActor(Ref<Actor> actor);
		private:
			Ref<Primitive> screenRect;
		};
	}
}

#endif

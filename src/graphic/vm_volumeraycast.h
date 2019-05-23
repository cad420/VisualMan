
#ifndef _VOLUMERAYCAST_H_
#define _VOLUMERAYCAST_H_
#include "visualman.h"
#include "graphictype.h"
#include "primitive.h"
#include "abstraarray.h"

namespace ysl
{
	namespace vm
	{

		class GRAPHICS_EXPORT_IMPORT RayCastActorEventCallback:public IActorEvent
		{
		public:
			RayCastActorEventCallback();
			void OnActorRenderStartedEvent(Actor* actor, const Camera* camera, Renderable* renderable, const Shading* shading, int pass) override;
			void BindToActor(Ref<Actor> actor);
		private:
			Ref<Primitive> proxyGeometry;
			Ref<ArrayFloat3> vertexArray;
			Ref<ArrayFloat3> texCoordArray;
		};


		class GRAPHICS_EXPORT_IMPORT VM_VolumeRayCast:public VisualMan
		{
		public:
			VM_VolumeRayCast() = default;
			void UpdateScene() override;
			void InitEvent() override;
		private:
		};
	}
}

#endif

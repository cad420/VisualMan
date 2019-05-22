
#ifndef _VM_MESH_H_
#define _VM_MESH_H_
#include "visualman.h"

namespace ysl
{
	namespace vm
	{

		class GRAPHICS_EXPORT_IMPORT MarchingCubeActorCallback:public IActorEvent
		{
		public:
			void OnActorRenderStartedEvent(Actor* actor, const Camera* camera, Renderable* renderable, const Shading* shading, int pass) override;

		};

		class GRAPHICS_EXPORT_IMPORT VM_Mesh :public VisualMan
		{
		public:
			void InitEvent() override;
			void UpdateScene() override;
		};
	}

}
#endif
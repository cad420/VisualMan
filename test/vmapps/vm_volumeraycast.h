
#ifndef _VOLUMERAYCAST_H_
#define _VOLUMERAYCAST_H_
#include "visualman.h"
#include "graphictype.h"
#include "primitive.h"
#include "abstraarray.h"
#include "trivialscenemanager.h"

namespace ysl
{
	namespace vm
	{

		class RayCastActorEventCallback;

		class VISUALMAN_EXPORT_IMPORT VM_VolumeRayCast:public VisualMan
		{
		public:
			VM_VolumeRayCast() = default;
			void UpdateScene() override;
			void InitEvent() override;
		private:
			void AddBoundingBox();

			Ref<RayCastActorEventCallback> rayCastCallback;
			Ref<TrivialSceneManager> sceneManager;
		};
	}
}

#endif

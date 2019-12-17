
#ifndef _VOLUMERAYCAST_H_
#define _VOLUMERAYCAST_H_
#include "visualman.h"
#include "config.h"
#include "graphictype.h"
#include "trivialscenemanager.h"

	namespace vm
	{

		class RayCastActorEventCallback;

		class VMAPPS_EXPORT_IMPORT VM_VolumeRayCast:public VisualMan
		{
		public:
			VM_VolumeRayCast() = default;
			void UpdateScene() override;
			void InitEvent() override;
		private:
			void AddBoundingBox();

			VMRef<RayCastActorEventCallback> rayCastCallback;
			VMRef<TrivialSceneManager> sceneManager;
		};
	}

#endif


#ifndef _LARGEVOLUMERAYCAST_H_
#define _LARGEVOLUMERAYCAST_H_

#include <visualman.h>
#include "config.h"
#include "oocprimitive.h"

namespace ysl
{
	namespace vm
	{
		class VMAPPS_EXPORT_IMPORT VM_LargeVolumeRayCast:public VisualMan
		{
		public:
			void InitEvent() override;
			void UpdateScene() override;
			void DestroyEvent() override;
			void FileDropEvent(const std::vector<std::string>& fileNames) override;
			void ResizeEvent(int w, int h) override;
			void MouseMoveEvent(MouseButton button, int xpos, int ypos) override;
			void UpdateEvent() override;
		private:
			void SetupShading();
			void SetupResources(const std::string& fileName);
			void SetupTF(const std::string & fileName);
			Ref<Shading> rayCastShading;
			Ref<Aggregate> mrtAgt, raycastAgt;
			Ref<OutOfCorePrimitive> oocPrimitive;
		};
	}
}

#endif

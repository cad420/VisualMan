
#ifndef _LARGEVOLUMERAYCAST_H_
#define _LARGEVOLUMERAYCAST_H_

#include <visualman.h>
#include "config.h"

namespace ysl
{
	namespace vm
	{
		class VMAPPS_EXPORT_IMPORT VM_LargeVolumeRayCast:public VisualMan
		{
		public:
			void InitEvent() override;
			void UpdateScene() override;
		private:
			void SetupShading();
			void SetupResources();
		};
	}
}

#endif

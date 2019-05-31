
#ifndef _LARGEVOLUMERAYCAST_H_
#define _LARGEVOLUMERAYCAST_H_

#include "graphictype.h"
#include "visualman.h"

namespace ysl
{
	namespace vm
	{
		class GRAPHICS_EXPORT_IMPORT VM_LargeVolumeRayCast:public VisualMan
		{
		public:
			void InitEvent() override;
			void UpdateScene() override;
		private:
			void SetupShading();
		};
	}
}

#endif

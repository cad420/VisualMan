
#ifndef _VM_VOLUMERAYCAST2_H_
#define _VM_VOLUMERAYCAST2_H_

#include "visualman.h"
#include "config.h"

namespace ysl
{
	namespace vm
	{

		class VMAPPS_EXPORT_IMPORT VM_VolumeRayCast2:public VisualMan
		{
		public:
			VM_VolumeRayCast2() = default;
			void InitEvent() override;
		private:
			void SetupAggregate();				// This may be a virtual function
			Ref<Texture> entryTexture;
			Ref<Texture> exitTexture;
		};
	}
}

#endif
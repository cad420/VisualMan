
#ifndef _VM_MESH_H_
#define _VM_MESH_H_
#include "visualman.h"
#include "primitive.h"
#include "config.h"

namespace ysl
{
	namespace vm
	{
		class VMAPPS_EXPORT_IMPORT VM_Mesh :public VisualMan
		{
		public:
			void InitEvent() override;
			void UpdateScene() override;
		private:
			Ref<Primitive> bunnyPrimitive;
		};
	}

}
#endif
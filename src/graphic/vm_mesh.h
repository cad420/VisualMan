
#ifndef _VM_MESH_H_
#define _VM_MESH_H_
#include "visualman.h"

namespace ysl
{
	namespace vm
	{
		class VM_Mesh :public VisualMan
		{
		public:
			void InitEvent() override;
			void UpdateScene() override;
		};
	}

}
#endif
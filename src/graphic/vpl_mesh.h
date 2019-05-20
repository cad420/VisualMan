
#ifndef _VPL_MESH_H_
#define _VPL_MESH_H_
#include "assembly.h"

namespace ysl
{
	namespace vpl
	{
		class VPL_Mesh :public Assembly
		{
		public:
			void InitEvent() override;
			void UpdateScene() override;
		};
	}

}
#endif
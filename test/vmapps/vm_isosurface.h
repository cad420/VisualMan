
#ifndef _ISOSURFACE_H_
#define _ISOSURFACE_H_
#include "visualman.h"
#include "primitive.h"

#include <marchingcubes.h>
#include "config.h"

namespace ysl
{
	namespace vm
	{
		class VMAPPS_EXPORT_IMPORT VM_ISOSurface :public VisualMan
		{
		public:
			void InitEvent() override;
			void UpdateScene() override;
			void SetNewData(const unsigned char * data,const Size3  & size);
			void KeyPressEvent(KeyButton key) override;
		private:
			
			void SetupPrimitive();
			void UpdatePrimitive();

			Ref<MeshGenerator> meshGen = nullptr;
			Ref<Primitive> surface = nullptr;
			std::vector<unsigned char> buffer;
			bool changed = true;
			int isoValue = 128;
		};
	}
}

#endif

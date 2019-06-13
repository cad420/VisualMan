
#ifndef _OOCRESOURCES_H_
#define _OOCRESOURCES_H_
#include "config.h"
#include "oocprimitive.h"
#include <texture.h>
#include "largevolumecache.h"

namespace ysl
{
	namespace vm
	{
		class VMAPPS_EXPORT_IMPORT OutOfCoreResources:public IOutOfCoreCallback
		{
		public:
			OutOfCoreResources() = default;
			OutOfCoreResources(const std::string & fileName);
			void OnDrawCallFinished(OutOfCorePrimitive*p) override;
			void OnDrawCallStart(OutOfCorePrimitive* p) override;
			Ref<Texture> GetVolumeTexture() { return videoMemoryData; }
			Ref<const Texture> GetVolumeTexture()const { return videoMemoryData;}
			void BindPrimitive(Ref<OutOfCorePrimitive> primitive);
		private:
			Ref<Texture> videoMemoryData;
			ExclRef<CPUVolumeDataCache> cpuMemoryData;
			// resources
		};
	}
}
#endif

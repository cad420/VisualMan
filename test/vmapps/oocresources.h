
#ifndef _OOCRESOURCES_H_
#define _OOCRESOURCES_H_

#include "config.h"
#include "oocprimitive.h"
#include <texture.h>
#include <VMFoundation/largevolumecache.h>

namespace ysl
{
	namespace vm
	{
		class VMAPPS_EXPORT_IMPORT OutOfCoreResources:public IOutOfCoreAdapter			// Decorator Pattern
		{
		public:
			OutOfCoreResources() = default;
			OutOfCoreResources(const std::string & fileName);
			void OnDrawCallFinished(OutOfCorePrimitive*p) override;
			void OnDrawCallStart(OutOfCorePrimitive* p) override;
			//Ref<Texture> GetVolumeTexture() { return videoMemoryData; }
			//Ref<const Texture> GetVolumeTexture()const { return videoMemoryData;}
			void BindPrimitive(Ref<OutOfCorePrimitive> primitive);
		private:
			//Ref<Texture> videoMemoryData;
			//Ref<Texture> blockMapTable;
			//ExclRef<CPUVolumeDataCache> cpuMemoryData;

			// The key point is that how to implement a fast data upload from cpu. 
			// Thinking about uses of the PBO, Maybe a ping-pong transfer is an ideal way with it.
		};
	}
}
#endif

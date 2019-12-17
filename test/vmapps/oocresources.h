
#ifndef _OOCRESOURCES_H_
#define _OOCRESOURCES_H_

#include "config.h"
#include "oocprimitive.h"
#include <texture.h>
#include <VMFoundation/largevolumecache.h>

namespace vm
{
class VMAPPS_EXPORT_IMPORT OutOfCoreResources : public IOutOfCoreAdapter  // Decorator Pattern
{
public:
	OutOfCoreResources() = default;
	OutOfCoreResources( const std::string &fileName );
	void OnDrawCallFinished( OutOfCorePrimitive *p ) override;
	void OnDrawCallStart( OutOfCorePrimitive *p ) override;
	//VMRef<Texture> GetVolumeTexture() { return videoMemoryData; }
	//VMRef<const Texture> GetVolumeTexture()const { return videoMemoryData;}
	void BindPrimitive( VMRef<OutOfCorePrimitive> primitive );

private:
	//VMRef<Texture> videoMemoryData;
	//VMRef<Texture> blockMapTable;
	//ExclRef<CPUVolumeDataCache> cpuMemoryData;

	// The key point is that how to implement a fast data upload from cpu.
	// Thinking about uses of the PBO, Maybe a ping-pong transfer is an ideal way with it.
};
}  // namespace vm
#endif


#ifndef _ISOSURFACE_H_
#define _ISOSURFACE_H_
#include "visualman.h"
#include "primitive.h"

#include <VMGraphics/marchingcubes.h>
#include "config.h"

namespace vm
{
class VMAPPS_EXPORT_IMPORT VM_ISOSurface : public VisualMan
{
public:
	void InitEvent() override;
	void UpdateScene() override;
	void SetNewData( const unsigned char *data, const Size3 &size );
	void KeyPressEvent( KeyButton key ) override;

private:
	void SetupPrimitive();
	void UpdatePrimitive();

	VMRef<MeshGenerator> meshGen = nullptr;
	VMRef<Primitive> surface = nullptr;
	std::vector<unsigned char> buffer;
	bool changed = true;
	int isoValue = 128;
};
}  // namespace vm

#endif

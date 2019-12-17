
#ifndef _VM_MESH_H_
#define _VM_MESH_H_
#include <visualman.h>
#include <primitive.h>
#include "config.h"

namespace vm
{
class VMAPPS_EXPORT_IMPORT VM_Mesh : public VisualMan
{
public:
	void InitEvent() override;
	void UpdateScene() override;

private:
	VMRef<Primitive> bunnyPrimitive;
};
}  // namespace vm

#endif
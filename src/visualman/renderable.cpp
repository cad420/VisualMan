
#include "renderable.h"
#include <iostream>

namespace ysl
{
namespace vm
{
void Renderable::Render( const Actor *actor, const Shading *shading, const Camera *camera, RenderContext *context )
{
	// Check renderable whether is dirty
	if ( GetBufferObjectEnabled() && GetBufferObjectDirty() ) {
		// Update Buffer Object
		UpdateDirtyBufferObject( VM_UM_KeepRAM );
		SetBufferObjectDirty( false );
	}
	Render_Implement( actor, shading, camera, context );
}
}  // namespace vm
}  // namespace ysl


#include "oocprimitive.h"
#include "rendercontext.h"

namespace vm
{
OutOfCorePrimitive::OutOfCorePrimitive()
{
	class DefaultOutOfCoreCallback : public IOutOfCoreAdapter
	{
	public:
		void OnDrawCallStart( OutOfCorePrimitive *p ) override { p->SetRenderFinished( false ); }
		void OnDrawCallFinished( OutOfCorePrimitive *p ) override { p->SetRenderFinished( true ); }
	};
	SetOutOfCoreResources( MakeVMRef<DefaultOutOfCoreCallback>() );
}

void OutOfCorePrimitive::Render( const Actor *actor, const Shading *shading, const Camera *camera,
								 RenderContext *context )
{
	// Check renderable whether is dirty
	if ( GetBufferObjectEnabled() && GetBufferObjectDirty() ) {
		UpdateDirtyBufferObject( VM_UM_KeepRAM );
		SetBufferObjectDirty( false );
	}

	renderFinished = false;
	do {
		callbacks->OnDrawCallStart( this );
		if ( renderFinished )
			break;
		Render_Implement( actor, shading, camera, context );
		callbacks->OnDrawCallFinished( this );
		if ( renderFinished )
			break;

	} while ( !renderFinished );
}

void OutOfCorePrimitive::SetOutOfCoreResources( VMRef<IOutOfCoreAdapter> e )
{
	callbacks = e;
}

void OutOfCorePrimitive::RemoveOutOfCoreResources( VMRef<IOutOfCoreAdapter> e )
{
	callbacks = nullptr;
}

void OutOfCorePrimitive::RemoveOutOfCoreResources()
{
	callbacks = nullptr;
}
}  // namespace vm
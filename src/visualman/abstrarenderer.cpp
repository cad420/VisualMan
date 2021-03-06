
#include "abstrarenderer.h"
#include "renderevent.h"

namespace vm
{
void AbstraRenderer::DispatchOnRenderStartedEvent()
{
	//Debug("AbstraRenderer::DispatchOnRenderStartedEvent\n");

	for ( auto it = startedCallbacks.begin(); it != startedCallbacks.end(); ) {
		auto &item = *it;
		if ( item->IsEnabled() && item->OnRendererStartedEvent( this ) && item->IsRemoveAfterCallEnabled() )
			it = startedCallbacks.erase( it );
		else
			++it;
	}
}

void AbstraRenderer::DispatchOnRenderFinishedEvent()
{
	//Debug("AbstraRenderer::DispatchOnRenderFinishedEent\n");

	for ( auto it = finishedCallbacks.begin(); it != finishedCallbacks.end(); ) {
		auto &item = *it;
		if ( item->IsEnabled() && item->OnRendererFinishedEvent( this ) && item->IsRemoveAfterCallEnabled() )
			it = finishedCallbacks.erase( it );
		else
			++it;
	}
}

void AbstraRenderer::AddRenderStartedEventCallback( VMRef<IRenderEvent> callback )
{
	startedCallbacks.push_back( std::move( callback ) );
}

void AbstraRenderer::AddRenderFinishedEventCallback( VMRef<IRenderEvent> callback )
{
	finishedCallbacks.push_back( std::move( callback ) );
}
}  // namespace vm

//#include "assembly.h"
#include "visualman.h"
#include "rendercontext.h"
#include <VMUtils/log.hpp>

namespace vm
{
void VisualMan::InitDefault()
{
	auto aggre = MakeVMRef<Aggregate>();
	abstraAggregate = aggre;
	// A frame has created a camera,
	//We just need to bind it to the manipulator
	manipulator->SetCamera( aggre->CreateGetCamera() );
}
VisualMan::VisualMan()
{
	manipulator = MakeVMRef<CameraManipulator>();
}

void VisualMan::DestroyEvent()
{
	manipulator.reset();
	abstraAggregate.reset();
}

void VisualMan::UpdateEvent()
{
	timer.stop();
	const auto dur = timer.duration().s();
	if ( dur > 1.0 ) 
	{
		FPS = frameCount / dur;
		frameCount = 0;
		if ( fpsResult.size() >= 50 ) {
			std::cout << "Full\n";
			fpsResult.pop_front();
		}
		fpsResult.push_back( FPS );
		timer.start();
	}
	frameCount++;
	//Update Scene
	UpdateScene();
	// execute rendering
	assert( abstraAggregate );
	abstraAggregate->Render();
	// swap buffer
	if ( Context()->HasDoubleBuffer() )
		Context()->SwapBuffer();
}

void VisualMan::AddedEvent( RenderContext *context )
{
	// The object has been added into context
	// Here, you need to do something depended this event. e.g. Set camera event interface.
	assert( context );
	context->AddEventListener( manipulator );
}

void VisualMan::DeletedEvent( RenderContext *context )
{
	// Here, you need to do something done at AddedEvent(RenderContext*) in contrary manners
	assert( context );
	assert( manipulator );
	context->RemoveEventListener( manipulator );
}

void VisualMan::MousePressEvent( MouseButton button, int xpos, int ypos )
{
}

void VisualMan::MouseReleaseEvent( MouseButton button, int xpos, int ypos )
{
}

void VisualMan::MouseMoveEvent( MouseButton button, int xpos, int ypos )
{
}

void VisualMan::MouseWheelEvent( int ydegree, int xdegree )
{
}

void VisualMan::KeyPressEvent( KeyButton key )
{
}

void VisualMan::KeyReleaseEvent( KeyButton key )
{
}

void VisualMan::ResizeEvent( int w, int h )
{
	auto aggr = std::dynamic_pointer_cast<Aggregate>( GetAggregate() );
	if ( aggr ) {
		aggr->CreateGetCamera()->GetViewport()->SetWidth( w );
		aggr->CreateGetCamera()->GetViewport()->SetHeight( h );
	}
}

void VisualMan::FileDropEvent( const std::vector<std::string> &fileNames )
{
	(void)fileNames;
}

void VisualMan::BindCameraEvent( VMRef<Camera> camera )
{
	manipulator->SetCamera( camera );
}


}  // namespace vm

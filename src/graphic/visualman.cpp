//#include "assembly.h"
#include "visualman.h"
#include <iostream>

#include "rendercontext.h"
#include "../utility/error.h"

namespace ysl
{
	namespace vm
	{
		void VisualMan::InitDefault()
		{
			frame = MakeRef<Frame>();
			// A frame has created a camera, 
			//We just need to bind it to the manipulator
			manipulator->SetCamera(frame->GetCamera());
		}

		VisualMan::VisualMan()
		{
			manipulator = MakeRef<CameraManipulator>();
		}

		void VisualMan::DestroyEvent()
		{
			std::cout << "Assembly::DestroyEvent\n";
		}

		void VisualMan::UpdateEvent()
		{
			//Update Scene
			UpdateScene();
			// execute rendering
			assert(frame);
			frame->Render();
			// swap buffer
			if (Context()->HasDoubleBuffer())
				Context()->SwapBuffer();
		}

		void VisualMan::AddedEvent(RenderContext* context)
		{
			// The object has been added into context
			// Here, you need to do something depended this event. e.g. Set camera event interface.
			assert(context);
			context->AddEventListener(manipulator);
		}

		void VisualMan::DeletedEvent(RenderContext* context)
		{
			// Here, you need to do something done at AddedEvent(RenderContext*) in contrary manners

			assert(context);
			assert(manipulator);
			context->RemoveEventListener(manipulator);
		}

		void VisualMan::MousePressEvent(EMouseButton button, int xpos, int ypos)
		{
			//std::cout << "Assembly::MousePressEvent:" << button << " " << xpos << " " << ypos << std::endl;
		}

		void VisualMan::MouseReleaseEvent(EMouseButton button, int xpos, int ypos)
		{
			//std::cout << "Assembly::MouseReleaseEvent:"<<button<<" "<<xpos<<" "<<ypos<< std::endl;
		}

		void VisualMan::MouseMoveEvent(EMouseButton button, int xpos, int ypos)
		{
			//std::cout << "Assembly::MouseMoveEvent:" << button << " " << xpos << " " << ypos << std::endl;
		}

		void VisualMan::MouseWheelEvent(int ydegree, int xdegree)
		{
			//std::cout << "Assembly::MouseWheelEvent:" << xdegree << " " << ydegree << std::endl;
		}

		void VisualMan::KeyPressEvent(EKeyButton key)
		{
			//std::cout << "Assembly::MouseReleaseEvent:"<<key << std::endl;
		}

		void VisualMan::KeyReleaseEvent(EKeyButton key)
		{
			//std::cout << "Assembly::KeyReleaseEvent:"<< key<<  std::endl;
		}

		void VisualMan::ResizeEvent(int w, int h)
		{
			std::cout << "Assembly::ResizeEvent:"<<w<<" "<<h << std::endl;
			//if(frame->GetCamera())
			//	frame->GetCamera()->GetViewport()->SetViewportSize(w, h);
			//frame->GetCamera()->GetViewport()->Activate();

		}
	}
}

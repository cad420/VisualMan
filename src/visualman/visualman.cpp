//#include "assembly.h"
#include "visualman.h"
#include "rendercontext.h"
#include <error.h>

namespace ysl
{
	namespace vm
	{
		void VisualMan::InitDefault()
		{
			auto aggre = MakeRef<Aggregate>();
			abstraAggregate = aggre;
			// A frame has created a camera, 
			//We just need to bind it to the manipulator
			manipulator->SetCamera(aggre->CreateGetCamera());
		}
		VisualMan::VisualMan()
		{
			manipulator = MakeRef<CameraManipulator>();
		}

		void VisualMan::DestroyEvent()
		{
			manipulator.reset();
			abstraAggregate.reset();
		}

		void VisualMan::UpdateEvent()
		{
			timer.end();
			const auto dur = timer.duration()*1.0/1000000;
			if(dur > 0.5)
			{
				FPS = frameCount / dur;
				frameCount = 0;
				timer.begin();
			}
			frameCount++;
			//Update Scene
			UpdateScene();
			// execute rendering
			assert(abstraAggregate);
			abstraAggregate->Render();
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

		void VisualMan::MousePressEvent(MouseButton button, int xpos, int ypos)
		{
			//std::cout << "Assembly::MousePressEvent:" << button << " " << xpos << " " << ypos << std::endl;
		}

		void VisualMan::MouseReleaseEvent(MouseButton button, int xpos, int ypos)
		{
			//std::cout << "Assembly::MouseReleaseEvent:"<<button<<" "<<xpos<<" "<<ypos<< std::endl;
		}

		void VisualMan::MouseMoveEvent(MouseButton button, int xpos, int ypos)
		{
			//std::cout << "Assembly::MouseMoveEvent:" << button << " " << xpos << " " << ypos << std::endl;
		}

		void VisualMan::MouseWheelEvent(int ydegree, int xdegree)
		{
			//std::cout << "Assembly::MouseWheelEvent:" << xdegree << " " << ydegree << std::endl;
		}

		void VisualMan::KeyPressEvent(KeyButton key)
		{
			//std::cout << "Assembly::MouseReleaseEvent:"<<key << std::endl;
		}

		void VisualMan::KeyReleaseEvent(KeyButton key)
		{
			//std::cout << "Assembly::KeyReleaseEvent:"<< key<<  std::endl;
		}

		void VisualMan::ResizeEvent(int w, int h)
		{
			auto aggr = std::dynamic_pointer_cast<Aggregate>(GetAggregate());
			if(aggr)
			{
				aggr->CreateGetCamera()->GetViewport()->SetWidth(w);
				aggr->CreateGetCamera()->GetViewport()->SetHeight(h);
			}
		}

		void VisualMan::FileDropEvent(const std::vector<std::string>& fileNames)
		{
			(void)fileNames;
		}

		void VisualMan::BindCameraEvent(Ref<Camera> camera)
		{
			manipulator->SetCamera(camera);
		}
	}
}

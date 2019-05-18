//#include "assembly.h"
#include "assembly.h"
#include <iostream>

#include "rendercontext.h"
#include "../utility/error.h"

namespace ysl
{
	namespace graphics
	{
		void Assembly::InitEvent()
		{
			std::cout << "Assembly::InitEvent\n";
			studio = MakeRef<Frame>();
			ysl::Log("Render Studio has been created\n");
		}

		void Assembly::DestroyEvent()
		{
			std::cout << "Assembly::DestroyEvent\n";
		}

		void Assembly::UpdateEvent()
		{
			std::cout << "Assembly::UpdateEvent\n";

			//Update Scene

			//UpdateScene();

			// execute rendering
			assert(studio);
			studio->Render();

			// swap buffer
			if (Context()->HasDoubleBuffer())
				Context()->SwapBuffer();
		}

		void Assembly::AddedEvent(RenderContext* context)
		{
			// The object has been added into context
			// Here, you need to do something depended this event. e.g. Set camera event interface.


		}

		void Assembly::DeletedEvent(RenderContext* context)
		{
			// Here, you need to do something done at AddedEvent(RenderContext*) in contrary manners

		}

		void Assembly::MousePressEvent(EMouseButton button, int xpos, int ypos)
		{
			std::cout << "Assembly::MousePressEvent:" << button << " " << xpos << " " << ypos << std::endl;
		}

		void Assembly::MouseReleaseEvent(EMouseButton button, int xpos, int ypos)
		{
			std::cout << "Assembly::MouseReleaseEvent:"<<button<<" "<<xpos<<" "<<ypos<< std::endl;
		}

		void Assembly::MouseMoveEvent(EMouseButton button, int xpos, int ypos)
		{
			std::cout << "Assembly::MouseMoveEvent:" << button << " " << xpos << " " << ypos << std::endl;
		}

		void Assembly::MouseWheelEvent(int ydegree, int xdegree)
		{
			std::cout << "Assembly::MouseWheelEvent:" << xdegree << " " << ydegree << std::endl;
		}

		void Assembly::KeyPressEvent(EKeyButton key)
		{
			std::cout << "Assembly::MouseReleaseEvent:"<<key << std::endl;
		}

		void Assembly::KeyReleaseEvent(EKeyButton key)
		{
			std::cout << "Assembly::KeyReleaseEvent:"<< key<<  std::endl;
		}

		void Assembly::ResizeEvent(int w, int h)
		{
			std::cout << "Assembly::ResizeEvent:"<<w<<" "<<h << std::endl;
		}
	}
}

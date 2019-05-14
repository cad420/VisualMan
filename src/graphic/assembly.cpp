//#include "assembly.h"
#include "assembly.h"
#include <iostream>

namespace ysl
{
	namespace gpu
	{
		void Assembly::InitEvent()
		{
			std::cout << "Assembly::InitEvent\n";
		}

		void Assembly::DestroyEvent()
		{
			std::cout << "Assembly::DestroyEvent\n";
		}

		void Assembly::UpdateEvent()
		{
			std::cout << "Assembly::UpdateEvent\n";
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

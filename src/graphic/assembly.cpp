
#include "assembly.h"
#include <iostream>

namespace ysl
{
	namespace gpu
	{
		void RenderAssembly::UpdateEvent()
		{
			std::cout << "RenderAssembly::UpdateEvent()\n";

		}

		void RenderAssembly::MousePressEvent(MouseButton button, int xpos, int ypos)
		{
			std::cout << "RenderAssembly::MousePressEvent" << std::endl;
		}

		void RenderAssembly::MouseReleaseEvent(MouseButton button, int xpos, int ypos)
		{
			std::cout << "RenderAssembly::MouseReleaseEvent" << std::endl;
		}

		void RenderAssembly::ResizeEvent(int w, int h)
		{
			std::cout << "RenderAssembly::ResizeEvent:"<<w<<", "<<h << std::endl;

		}
	}
}

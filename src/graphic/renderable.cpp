
#include "renderable.h"
#include <iostream>

namespace ysl
{
	namespace gpu
	{
		void Renderable::Render(const Actor* actor, const RenderState * state, const Camera * camera, RenderContext * context)
		{
			std::cout << "Renderable::Render\n";
			Render_Implement(actor,state,camera,context);
		}
	}
}

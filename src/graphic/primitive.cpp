
#include "primitive.h"
#include <iostream>

namespace ysl
{
	namespace graphics
	{
		void Primitive::AddDrawCall(Ref<AbstraDrawCall> dc)
		{
		}

		void Primitive::Render_Implement(const Actor * actor, const RenderState* state, const Camera* camera, RenderContext * context)
		{
			std::cout << "Primitive::Render_Implement";

			// Bind vbo interface to context
			// A primitive is an implementation of the VertexArray Interface 

			// call draw command

		}
	}
}

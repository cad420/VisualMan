
#include "renderer.h"
#include <iostream>

namespace ysl
{
	namespace gpu
	{
		void Renderer::Render(const std::vector<Ref<Actor>> & rederQueue,
			const Ref<Camera>& camera)
		{

			std::cout << "Renderer::Render has been called\n";


			// active framebuffer and get context from fb

			// set render state for the fb

			// dispatch before-render events

			// render the queue

			/// for every token and for each lod
			/// set shader state and enable for the token
			/// dispatch actor before-render event
			/// set glsl , cameras, uniforms dependent on current state
			/// if the projection matrix has been changed, dispatch corresponding events
			/// finally render the renderable object

			// clear enables
			// clear render states




			// dispatch after-render events;


		}
	}
}

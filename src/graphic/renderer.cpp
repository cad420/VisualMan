
#include "renderer.h"
#include <iostream>
#include "../opengl/openglutils.h"

namespace ysl
{
	namespace graphics
	{
		void Renderer::Render(const RenderQueue & rederQueue,
			const Ref<Camera>& camera)
		{

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

			std::cout << "Renderer::Render has been called\n";
			class Raii
			{
				Renderer * renderer;
				std::vector<RenderState> oldRenderStates;
			public:
				Raii(Renderer * renderer, Camera * camera) :renderer(renderer)
				{
					renderer->DispatchOnRenderStartedEvent();
					GL_CHECK
				}
				~Raii()
				{
					renderer->DispatchOnRenderFinishedEvent();
					GL_CHECK
				} 
			}raii(this,camera.get());




			return;
		}
	}
}

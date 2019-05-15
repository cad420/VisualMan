
#include "renderstudio.h"
#include "renderer.h"
#include "rendercontext.h"
#include "abstrarenderer.h"
#include "../utility/error.h"
#include "../opengl/openglutils.h"

namespace ysl
{
	namespace graphics
	{
		void RenderStudio::Render()
		{
			std::cout << "Render Loop\n";

			// calls for a renderer

			assert(!renderers.empty());

			class Raii
			{
				RenderStudio * const studio;
				RenderContext * context;
			public:
				Raii(RenderStudio * studio):studio(studio)
				{
					
					if(studio->renderers.empty())
					{
						ysl::Log("No specified renderer\n");
						return;
					}
					if(!studio->renderers[0]->GetFramebuffer())
					{
						ysl::Log("The Renderer has no specified framebuffer");
						return;
					}
					context = studio->renderers[0]->GetFramebuffer()->Context();
					if(!context)
					{
						ysl::Error("The framebuffer has no corresponding context");
						return;
					}

					context->MakeCurrent(); // Redundant ??
					context->SetContextState(Context_OnRenderingStarted);
					studio->DispatchRenderStartedEvent();
					GL_CHECK
				}
				~Raii()
				{
					context->SetContextState(Context_OnRenderingFinished);
					studio->DispatchRenderFinishedEvent();
					GL_CHECK
				}
				
			} raii(this);

			if (sceneManagers.empty()) return;

			if (!camera) return;

			// construct actor queue from scene manager
			for(const auto & scene:sceneManagers)
			{
				scene->AppendActors(actorQueue);
			}

			// construct render queue from actor queue
			auto renderQueue = MakeRenderQueue(actorQueue);



			// rendering the queue with all renderers

			for(auto & renderer:renderers)
			{

				// Check framebuffer and context for every renderer
				renderer->Render(renderQueue, camera);
			}

		}

		RenderQueue RenderStudio::MakeRenderQueue(const std::vector<Ref<Actor>>& queue)
		{
			std::cout << "RenderStudio::MakeRenderQueue\n";

			// For every actor

			// evaluate the LOD

			// Multi passing

			// for each program, link it

			// texture creation




			return RenderQueue{};


		}
	}
}

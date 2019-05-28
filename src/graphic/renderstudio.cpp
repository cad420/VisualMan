
#include "renderstudio.h"
#include "renderer.h"
#include "rendercontext.h"
#include "abstrarenderer.h"
#include "../utility/error.h"
#include "../opengl/openglutils.h"

namespace ysl
{
	namespace vm
	{
		void Aggregate::Render()
		{
			// calls for a renderer
			assert(!renderers.empty());

			class Raii
			{
				Aggregate * const studio;
				RenderContext * context;
			public:
				Raii(Aggregate * studio):studio(studio)
				{
					
					assert(!studio->renderers.empty());
					assert(studio->renderers[0]->GetFramebuffer());
					assert(context);

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
			actorQueue.clear();
			for(const auto & scene:sceneManagers)
			{
				scene->ExtractActors(actorQueue);
			}
			// construct render queue from actor queue
			const auto renderQueue = MakeRenderQueue(actorQueue);
			// rendering the queue with all renderers
			for(auto & renderer:renderers)
			{

				// Check framebuffer and context for every renderer
				renderer->Render(renderQueue, camera);
			}
		}
		RenderQueue Aggregate::MakeRenderQueue(const std::vector<Ref<Actor>>& queue)
		{

			// For every actor

			// evaluate the LOD

			// Multi passing


			RenderQueue renderQueue;
			for(const auto & actor:queue)
			{
				const auto artist = actor->GetArtist();
				assert(artist);
				if(artist)
				{
					const auto primitiveLOD = actor->EvalLod(camera.get());
					const auto renderable = actor->GetRenderable(primitiveLOD);
					const auto shadingLOD = artist->EvalLOD(actor.get(), camera.get());

					Ref<ShadingPasses> shadingPasses = artist->GetLOD(shadingLOD);

					RenderNode * node = nullptr;

					for(int i = 0 ; i< shadingPasses->size();i++)
					{
						auto shading = (*shadingPasses)[i];
						if(node)
						{
							assert(i > 0);


							auto nextNode = renderQueue.CreateMultiPass(node, 
								actor.get(), 
								renderable.get(), 
								shading.get());

							node = nextNode;
						}else
						{
							assert(i == 0);
							node = renderQueue.CreateRenderNode(actor.get(),
								renderable.get(),
								shading.get());

							
						}


						// for each render node, check its resources status. e.g. 
						// texture create, program link

						auto program = shading->GetProgram();
						assert(program);

						if (!program->Linked())
							program->Link();

						Ref<RenderStateSet> stateSet = shading->GetRenderStateSet();
						assert(stateSet);
						if (stateSet)
						{
							// find texture
							for (auto & each : stateSet->renderStates)
							{
								if (each.rawRenderState->Type() == RS_TextureSampler)
								{
									auto sampler = std::static_pointer_cast<TextureSampler>(each.rawRenderState);
									assert(sampler);
									auto tex = sampler->GetTexture();
									if (tex && !tex->Handle())
									{
										tex->CreateTexture();
									}
								}
							}

						}
					}
				}

			}
			return renderQueue;
		}
	}
}

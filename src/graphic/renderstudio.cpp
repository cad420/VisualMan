
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
				Aggregate * const aggregate;
				RenderContext * context;
			public:
				Raii(Aggregate * aggr):aggregate(aggr)
				{
					
					assert(!aggr->renderers.empty());
					assert(aggr->renderers[0]->GetFramebuffer());
					assert(context);

					if(aggr->renderers.empty())
					{
						ysl::Log("No specified renderer\n");
						return;
					}
					if(!aggr->renderers[0]->GetFramebuffer())
					{
						ysl::Log("The Renderer has no specified framebuffer");
						return;
					}
					context = aggr->renderers[0]->GetFramebuffer()->Context();
					if(!context)
					{
						ysl::Error("The framebuffer has no corresponding context");
						return;
					}
					context->MakeCurrent(); // Redundant ??
					context->SetContextState(Context_OnRenderingStarted);
					aggr->DispatchRenderStartedEvent();
					GL_CHECK
				}
				~Raii()
				{
					context->SetContextState(Context_OnRenderingFinished);
					aggregate->DispatchRenderFinishedEvent();
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
			auto renderQueue = MakeRenderQueue(actorQueue);

			renderQueue.Sort();			// Sorting by the priority of the actor in the render node.

			// rendering the queue with all renderers
			for(auto & renderer:renderers)		
			{
				// These renderers are serialized. Their orders are dependent.
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

		void SerializedAggregates::Render()
		{
			for (auto & aggregate : aggregates)
				aggregate->Render();
		}
	}
}

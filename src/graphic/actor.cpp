
#include "actor.h"
#include "renderable.h"
#include "shaderprogram.h"

namespace ysl
{
	namespace vm
	{
		void IActorEvent::OnActorRenderStartedEvent(Actor* actor, const Camera* camera, Renderable* renderable,
			const Shading* shading, int pass)
		{
			(void)actor;
			(void)camera;
			(void)renderable;
			(void)shading;
			(void)pass;
			
		}

		void IActorEvent::OnActorDeletingEvent(Actor* actor, const Camera* camera, Renderable* renderable,
			const Shading* shading, int pass)
		{
			(void)actor;
			(void)camera;
			(void)renderable;
			(void)shading;
			(void)pass;
		}

		//void IActorEvent::OnActorContinueRender(Actor* actor, const Camera* camera, Renderable* renderable,
		//	const Shading* shading, int pass,bool &continued)
		//{
		//	(void)actor;
		//	(void)camera;
		//	(void)renderable;
		//	(void)shading;
		//	(void)pass;
		//	continued = false;
		//}

		void MarchingCubeActorCallback::OnActorRenderStartedEvent(Actor* actor, const Camera* camera,
			Renderable* renderable, const Shading* shading, int pass)
		{
			if (shading)
			{
				auto program = shading->GetProgram();

				const int loc = program->GetGenericUniformLocation("view_pos");
				if (loc != -1)
				{
					auto view_pos = camera->Position();
					actor->CreateGetUniformSet()->CreateGetUniform("view_pos")->SetUniform3f(1, view_pos.Data());
				}
			}
		}

		Actor::Actor(Ref<Renderable> renderable, Ref<Artist> art, Ref<Transform> transform):
			artist(std::move(art))
		{
			if (transform == nullptr)
				this->transform = MakeRef<Transform>();
			else
				this->transform = std::move(transform);
			renderables[0] = std::move(renderable);
		}

		void Actor::DispatchOnActorRenderStartedEvent(const Camera * camera, Renderable * renderable, const Shading * shading, int pass)
		{
			for (const auto & each : actorEvents)
			{
				if (each->IsEventEnable())
					each->OnActorRenderStartedEvent(this, camera, renderable, shading, pass);
			}
		}

		void Actor::DispatchOnActorDeletingEvent(const Camera * camera, Renderable * renderable, const Shading * shading, int pass)
		{
			for (const auto & each : actorEvents)
			{
				each->OnActorDeletingEvent(this, camera, renderable, shading, pass);
			}
		}

		void Actor::AddActorRenderEventCallback(Ref<IActorEvent> callback)
		{
			actorEvents.push_back(std::move(callback));
		}

		void Actor::RemoveActorRenderEventCallback(Ref<IActorEvent> callback)
		{
			for(auto it = actorEvents.begin();it != actorEvents.end();)
			{
				if (*it == callback)
					it = actorEvents.erase(it);
				else
					++it;
			}
		}

		Ref<Renderable> Actor::GetRenderable(int lod)
		{
			assert(lod >= 0 && lod < 8);
			return renderables[lod];
		}

		void Actor::SetRenderable(Ref<Renderable> renderable, int lod)
		{
			assert(lod >= 0 && lod < 8);
			renderables[lod] = std::move(renderable);
		}

		int Actor::EvalLod(const Camera* camera)
		{
			if(lodEvaluator)
			{
				return lodEvaluator->Eval(this, camera);
			}
			return 0;
		}
	}
}

#ifndef _ACTOR_H_
#define _ACTOR_H_
#include "graphictype.h"
#include "uniform.h"
#include "art.h"
#include "../mathematics/transformation.h"

namespace ysl
{
	namespace vm 
	{
		class Renderable;

		class IActorEvent
		{
		public:
			virtual void OnActorRenderStartedEvent(Actor * actor,const Camera * camera,Renderable * renderable, const Shading * shading,int pass) = 0;
			virtual void OnActorDeletingEvent(Actor * actor, const Camera * camera, Renderable * renderable, const Shading * shading, int pass) = 0;
			void SetEventEnable(bool enable) { eventEnable = enable; }
			bool IsEventEnable() { return eventEnable; }
		private:
			bool eventEnable = true;
		};

		class  Actor
		{
		public:
			Actor(Ref<Renderable> renderable, Ref<Artist> art, Ref<Transform> transform);
			void SetTransform(Ref<Transform> transform){this->transform = std::move(transform);}
			Ref<Transform> GetTransform() { return transform; }
			void DispatchOnActorRenderStartedEvent(const Camera * camera,Renderable * renderable,const Shading * shading,int pass);
			void DispatchOnActorDeletingEvent(const Camera * camera, Renderable * renderable, const Shading * shading, int pass);
			void AddActorRenderEventCallback(Ref<IActorEvent> callback);
			void SetLODEvaluator(Ref<LODEvaluator> evaluator) { lodEvaluator = std::move(evaluator); }
			Ref<LODEvaluator> GetLODEvalutor() { return lodEvaluator; }

			Ref<UniformSet> GetUniformSet() { return uniformSet; }
			Ref<UniformSet> CreateUniformSet() { return uniformSet ? uniformSet : uniformSet = MakeRef<UniformSet>(); }
			void SetUniform(Ref<Uniform> uniform) { uniformSet->SetUniform(std::move(uniform)); }
			Ref<Uniform> GetUniform(const char * name) { return uniformSet->GetUniform(name); }

			Ref<Artist> GetArtist() { return artist; }

			Ref<Renderable> GetRenderableFromLod(int lod);

			int EvalLod(const Camera * camera);

			void RemoveUniform(const char * name) { return uniformSet->RemoveUniform(name); }
			void RemoveUniform(Ref<Uniform> uniform) { return uniformSet->RemoveUniform(uniform); }
		protected:
			Bound3f bound;
			Ref<UniformSet> uniformSet;
			Ref<Transform> transform;
			Ref<LODEvaluator> lodEvaluator;
			Ref<Artist> artist;
			std::vector<Ref<IActorEvent>> actorEvents;
			std::array<Ref<Renderable>, 8> renderables;
		};
	}
}

#endif
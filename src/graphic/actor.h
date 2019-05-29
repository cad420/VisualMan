
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

		class GRAPHICS_EXPORT_IMPORT IActorEvent:public std::enable_shared_from_this<IActorEvent>
		{
		public:
			virtual void OnActorRenderStartedEvent(Actor * actor,
				const Camera * camera,
				Renderable * renderable,
				const Shading * shading,
				int pass);
			virtual void OnActorDeletingEvent(Actor * actor,
				const Camera * camera,
				Renderable * renderable,
				const Shading * shading,
				int pass);

			void SetEventEnable(bool enable) { eventEnable = enable; }
			bool IsEventEnable()const { return eventEnable; }
			virtual ~IActorEvent() = default;
		private:
			bool eventEnable = true;
		};

		class GRAPHICS_EXPORT_IMPORT MarchingCubeActorCallback :public IActorEvent
		{
		public:
			void OnActorRenderStartedEvent(Actor* actor, const Camera* camera, Renderable* renderable, const Shading* shading, int pass) override;
		};

		class GRAPHICS_EXPORT_IMPORT Actor
		{
		public:
			Actor(Ref<Renderable> renderable, Ref<Artist> art, Ref<Transform> transform);
			void SetTransform(Ref<Transform> transform){this->transform = std::move(transform);}
			Ref<Transform> GetTransform() { return transform; }
			Ref<const Transform> GetTransform()const { return transform; }
			void DispatchOnActorRenderStartedEvent(const Camera * camera,Renderable * renderable,const Shading * shading,int pass);
			void DispatchOnActorDeletingEvent(const Camera * camera, Renderable * renderable, const Shading * shading, int pass);
			void AddActorRenderEventCallback(Ref<IActorEvent> callback);
			void RemoveActorRenderEventCallback(Ref<IActorEvent> callback);
			void SetLODEvaluator(Ref<LODEvaluator> evaluator) { lodEvaluator = std::move(evaluator); }

			Ref<LODEvaluator> GetLODEvaluator() { return lodEvaluator; }
			Ref<const LODEvaluator> GetLODEvaluator()const { return lodEvaluator; }

			Ref<UniformSet> GetUniformSet() { return uniformSet; }
			Ref<const UniformSet> GetUniformSet()const { return uniformSet; }
			Ref<UniformSet> CreateGetUniformSet() { return uniformSet ? uniformSet : uniformSet = MakeRef<UniformSet>(); }
			void SetUniform(Ref<Uniform> uniform) { uniformSet->SetUniform(std::move(uniform)); }
			Ref<Uniform> GetUniform(const char * name) { return uniformSet->GetUniform(name); }


			Ref<Artist> GetArtist() { return artist; }
			Ref<const Artist> GetArtist()const { return artist; }


			Ref<Renderable> GetRenderable(int lod);
			void SetRenderable(Ref<Renderable> renderable, int lod);


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
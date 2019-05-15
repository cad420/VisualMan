
#ifndef _ACTOR_H_
#define _ACTOR_H_
#include "graphictype.h"
#include "renderable.h"
#include "uniform.h"
#include "art.h"
#include "../mathematics/transformation.h"

namespace ysl
{
	namespace graphics 
	{

		class GRAPHICS_EXPORT_IMPORT IActorEvent
		{
		public:
			virtual void OnActorRenderStartedEvent() = 0;
			virtual void OnActorRenderFinishedEvent() = 0;
			void SetEventEnable(bool enable) { eventEnable = enable; }
			bool IsEventEnable() { return eventEnable; }
		private:
			bool eventEnable = true;
		};

		class GRAPHICS_EXPORT_IMPORT Actor
		{
		public:
			Actor() = default;
			void SetTransform(Ref<Transform> transform){this->transform = std::move(transform);}
			Ref<Transform> GetTransform() { return transform; }
			void DispatchOnActorRenderStartedEvent();
			void DispatchOnActorRenderFinishedEvent();
			void AddActorEventCallback(Ref<IActorEvent> callback);
			void SetLODEvaluator(Ref<LODEvaluator> evaluator) { lodEvaluator = std::move(evaluator); }
			Ref<LODEvaluator> GetLODEvalutor() { return lodEvaluator; }

			Ref<UniformSet> GetUniformSet() { return uniformSet; }
			Ref<UniformSet> CreateUniformSet() { return uniformSet ? uniformSet : uniformSet = MakeRef<UniformSet>(); }
			void SetUniform(Ref<Uniform> uniform) { uniformSet->SetUniform(std::move(uniform)); }
			Ref<Uniform> GetUniform(const char * name) { return uniformSet->GetUniform(name); }
			void RemoveUniform(const char * name) { return uniformSet->RemoveUniform(name); }
			void RemoveUniform(Ref<Uniform> uniform) { return uniformSet->RemoveUniform(uniform); }

		protected:
			Bound3f bound;
			Ref<Renderable> renderbles;
			Ref<UniformSet> uniformSet;
			Ref<Transform> transform;
			Ref<LODEvaluator> lodEvaluator;
			Ref<Art> art;
			std::vector<Ref<IActorEvent>> callbacks;

		};

	}
}

#endif
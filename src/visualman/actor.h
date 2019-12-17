
#ifndef _ACTOR_H_
#define _ACTOR_H_
#include "graphictype.h"
#include "uniform.h"
#include "art.h"
#include <VMat/transformation.h>

namespace vm
{
class Renderable;

class VISUALMAN_EXPORT_IMPORT IActorEvent : public std::enable_shared_from_this<IActorEvent>
{
public:
	virtual void OnActorRenderStartedEvent( Actor *actor,
											const Camera *camera,
											Renderable *renderable,
											const Shading *shading,
											int pass );
	virtual void OnActorDeletingEvent( Actor *actor,
									   const Camera *camera,
									   Renderable *renderable,
									   const Shading *shading,
									   int pass );
	//virtual void OnActorContinueRender(Actor * actor,const Camera * camera,Renderable * renderable,const Shading * shading,int pass,bool & continued);

	void SetEventEnable( bool enable ) { eventEnable = enable; }
	bool IsEventEnable() const { return eventEnable; }
	virtual ~IActorEvent() = default;

private:
	bool eventEnable = true;
};

class VISUALMAN_EXPORT_IMPORT MarchingCubeActorCallback : public IActorEvent
{
public:
	void OnActorRenderStartedEvent( Actor *actor, const Camera *camera, Renderable *renderable, const Shading *shading, int pass ) override;
};

class VISUALMAN_EXPORT_IMPORT Actor
{
public:
	Actor( VMRef<Renderable> renderable, VMRef<Artist> art, VMRef<Transform> transform );
	void SetTransform( VMRef<Transform> transform ) { this->transform = std::move( transform ); }
	VMRef<Transform> GetTransform() { return transform; }
	VMRef<const Transform> GetTransform() const { return transform; }
	void DispatchOnActorRenderStartedEvent( const Camera *camera, Renderable *renderable, const Shading *shading, int pass );
	void DispatchOnActorDeletingEvent( const Camera *camera, Renderable *renderable, const Shading *shading, int pass );
	//void DispatchOnActorContinueRenderEvent(const Camera * camera, Renderable * renderable, const Shading * shading, int pass);
	void AddActorRenderEventCallback( VMRef<IActorEvent> callback );
	void RemoveActorRenderEventCallback( VMRef<IActorEvent> callback );
	void SetLODEvaluator( VMRef<LODEvaluator> evaluator ) { lodEvaluator = std::move( evaluator ); }

	VMRef<LODEvaluator> GetLODEvaluator() { return lodEvaluator; }
	VMRef<const LODEvaluator> GetLODEvaluator() const { return lodEvaluator; }

	VMRef<UniformSet> GetUniformSet() { return uniformSet; }
	VMRef<const UniformSet> GetUniformSet() const { return uniformSet; }
	VMRef<UniformSet> CreateGetUniformSet() { return uniformSet ? uniformSet : uniformSet = MakeVMRef<UniformSet>(); }
	void SetUniform( VMRef<Uniform> uniform ) { uniformSet->SetUniform( std::move( uniform ) ); }
	VMRef<Uniform> GetUniform( const char *name ) { return uniformSet->GetUniform( name ); }

	VMRef<Artist> GetArtist() { return artist; }
	VMRef<const Artist> GetArtist() const { return artist; }

	VMRef<Renderable> GetRenderable( int lod );
	void SetRenderable( VMRef<Renderable> renderable, int lod );

	void SetPriority( int p ) { priority = p; }
	int GetPriority() const { return priority; }
	int EvalLod( const Camera *camera );
	void RemoveUniform( const char *name ) { return uniformSet->RemoveUniform( name ); }
	void RemoveUniform( VMRef<Uniform> uniform ) { return uniformSet->RemoveUniform( uniform ); }

protected:
	Bound3f bound;
	VMRef<UniformSet> uniformSet;
	VMRef<Transform> transform;
	VMRef<LODEvaluator> lodEvaluator;
	VMRef<Artist> artist;
	int priority = 0;
	std::vector<VMRef<IActorEvent>> actorEvents;
	std::array<VMRef<Renderable>, 8> renderables;
};
}  // namespace vm

#endif
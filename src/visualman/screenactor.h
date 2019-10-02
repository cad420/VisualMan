
#ifndef _SCREENACTOR_H_
#define _SCREENACTOR_H_
#include "actor.h"
#include "primitive.h"

namespace ysl
{
namespace vm
{
class VISUALMAN_EXPORT_IMPORT ScreenActorEventCallback : public IActorEvent
{
public:
	ScreenActorEventCallback( Ref<Camera> cam );
	void OnActorRenderStartedEvent( Actor *actor, const Camera *camera, Renderable *renderable, const Shading *shading, int pass ) override;
	void SetPrimitive( Ref<Primitive> primitive );
	void BindToActor( Ref<Actor> actor );

private:
	Ref<Primitive> screenRect;
	std::vector<float> lodTable;
	Ref<Camera> mrtCamera;
};
}  // namespace vm
}  // namespace ysl

#endif

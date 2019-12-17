
#ifndef _SCREENACTOR_H_
#define _SCREENACTOR_H_
#include "actor.h"
#include "primitive.h"

namespace vm
{
class VISUALMAN_EXPORT_IMPORT ScreenActorEventCallback : public IActorEvent
{
public:
	ScreenActorEventCallback( VMRef<Camera> cam );
	void OnActorRenderStartedEvent( Actor *actor, const Camera *camera, Renderable *renderable, const Shading *shading, int pass ) override;
	void SetPrimitive( VMRef<Primitive> primitive );
	void BindToActor( VMRef<Actor> actor );

private:
	VMRef<Primitive> screenRect;
	std::vector<float> lodTable;
	VMRef<Camera> mrtCamera;
};
}  // namespace vm

#endif

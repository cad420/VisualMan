
#ifndef _ACTOREVENTCALLBACK_H_
#define _ACTOREVENTCALLBACK_H_

#include "graphictype.h"
#include "actor.h"

namespace ysl
{
namespace vm
{
class Primitive;
class ArrayFloat3;

class VISUALMAN_EXPORT_IMPORT RayCastActorEventCallback : public IActorEvent
{
public:
	RayCastActorEventCallback();
	void OnActorRenderStartedEvent( Actor *actor, const Camera *camera, Renderable *renderable, const Shading *shading, int pass ) override;
	void BindToActor( Ref<Actor> actor );
	Ref<Primitive> ProxyGeometry() { return proxyGeometry; }

private:
	Ref<Primitive> proxyGeometry;
	Ref<ArrayFloat3> vertexArray;
	Ref<ArrayFloat3> texCoordArray;
};

class VISUALMAN_EXPORT_IMPORT RayCast2ActorEventCallback : public IActorEvent
{
public:
	RayCast2ActorEventCallback();
	void OnActorRenderStartedEvent( Actor *actor, const Camera *camera, Renderable *renderable, const Shading *shading, int pass ) override;
	void BindToActor( Ref<Actor> actor );
	Ref<Primitive> ProxyGeometry() { return proxyGeometry; }

private:
	Ref<Primitive> proxyGeometry;
	Ref<ArrayFloat3> vertexArray;
	Ref<ArrayFloat3> texCoordArray;
};

}  // namespace vm
}  // namespace ysl

#endif	// !1

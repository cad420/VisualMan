
#include "screenactor.h"
#include "abstraarray.h"
#include "drawarray.h"
#include "shaderprogram.h"
#include "camera.h"
#include "actor.h"

namespace ysl
{
namespace vm
{
ScreenActorEventCallback::ScreenActorEventCallback( Ref<Camera> cam )
{
	// Init a screen primitive
	SetPrimitive( MakeRef<Primitive>() );
	for ( int i = 0; i < 10; i++ ) {
		lodTable.push_back( 1.0 / std::pow( 8, 10 - i - 1 ) );
	}
	mrtCamera = std::move( cam );
}

void ScreenActorEventCallback::OnActorRenderStartedEvent( Actor *actor, const Camera *camera,
														  Renderable *, const Shading *shading, int )
{
	if ( mrtCamera ) {
		actor->CreateGetUniformSet()->CreateGetUniform( "fov" )->SetUniformValue( mrtCamera->GetFov() );
		actor->CreateGetUniformSet()->CreateGetUniform( "fuckPos" )->SetUniform3f( 1, mrtCamera->GetPosition().Data() );
	}
}

void ScreenActorEventCallback::SetPrimitive( Ref<Primitive> primitive )
{
	auto vertexNDC = MakeRef<ArrayFloat2>();
	auto texCoord = MakeRef<ArrayFloat2>();

	float v[] = {
		-1.f, 1.0f,
		-1.0f, -1.0f,
		1.0f, -1.0f,

		-1.0f, 1.0f,
		1.0f, -1.0f,
		1.0f, 1.0f
	};

	float t[] = {
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f
	};

	vertexNDC->GetBufferObject()->SetLocalData( v, sizeof( v ) );
	texCoord->GetBufferObject()->SetLocalData( t, sizeof( t ) );
	primitive->SetVertexPositionArray( vertexNDC );
	primitive->SetVertexTexCoordArray( texCoord );
	//draw call
	primitive->DrawCalls().push_back( MakeRef<DrawArray>( 0, 6 ) );
	screenRect = primitive;
}

void ScreenActorEventCallback::BindToActor( Ref<Actor> actor )
{
	const auto sharedThis = std::static_pointer_cast<ScreenActorEventCallback>( shared_from_this() );
	actor->RemoveActorRenderEventCallback( sharedThis );
	actor->AddActorRenderEventCallback( sharedThis );
	actor->SetRenderable( screenRect, 0 );
}
}  // namespace vm
}  // namespace ysl

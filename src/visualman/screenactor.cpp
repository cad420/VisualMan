
#include "screenactor.h"
#include "abstraarray.h"
#include "drawarray.h"
#include "shaderprogram.h"
#include "camera.h"
#include "actor.h"

namespace vm
{
ScreenActorEventCallback::ScreenActorEventCallback( VMRef<Camera> cam )
{
	// Init a screen primitive
	SetPrimitive( MakeVMRef<Primitive>() );
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

void ScreenActorEventCallback::SetPrimitive( VMRef<Primitive> primitive )
{
	auto vertexNDC = MakeVMRef<ArrayFloat2>();
	auto texCoord = MakeVMRef<ArrayFloat2>();

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
	primitive->DrawCalls().push_back( MakeVMRef<DrawArray>( 0, 6 ) );
	screenRect = primitive;
}

void ScreenActorEventCallback::BindToActor( VMRef<Actor> actor )
{
	const auto sharedThis = std::static_pointer_cast<ScreenActorEventCallback>( shared_from_this() );
	actor->RemoveActorRenderEventCallback( sharedThis );
	actor->AddActorRenderEventCallback( sharedThis );
	actor->SetRenderable( screenRect, 0 );
}
}  // namespace vm

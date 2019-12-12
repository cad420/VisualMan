
#ifndef _LARGEVOLUMERAYCAST_H_
#define _LARGEVOLUMERAYCAST_H_

#include <visualman.h>
#include <VMUtils/json_binding.hpp>
#include <vector>
#include <string>
#include "config.h"
#include "oocprimitive.h"
#include "drawelements.h"
#include <fstream>
#include "trivialscenemanager.h"

namespace ysl
{
namespace vm
{
struct LVDJSONStruct : ::vm::json::Serializable<LVDJSONStruct>
{
	VM_JSON_FIELD( std::vector<std::string>, fileNames );
	VM_JSON_FIELD( float, samplingRate );
	VM_JSON_FIELD( std::vector<float>, spacing );
};

class Primitive;
class ArrayFloat3;
class FrustumEventCallback : public IActorEvent
{
public:
	FrustumEventCallback( Ref<Camera> camera );
	void BindFrustumActor( Ref<Actor> actor );
	void OnActorRenderStartedEvent( Actor *actor, const Camera *camera, Renderable *renderable, const Shading *shading, int pass ) override;
	Ref<Primitive> GetFrustumPrimitive();

private:
	void UpdateFrustum();
	Ref<Primitive> frustum;
	Ref<Camera> camera;
	Ref<ArrayFloat3> vertexArray;
};

class BoundingBoxEventCallback : public IActorEvent
{
public:
	BoundingBoxEventCallback();
	void BindBoundingBoxActor( Ref<Actor> actor );
	void OnActorRenderStartedEvent( Actor *actor, const Camera *camera, Renderable *renderable, const Shading *shading, int pass ) override;

private:
	Ref<Primitive> boundingBox;
};

class NeroActorEventCallback : public IActorEvent
{
public:
	NeroActorEventCallback( const std::string &filename, const ysl::Vec3i &dataOriginalResolution )
	{
		neroGeometry = MakeRef<Primitive>();

		assert( neroActorCallback );
		//assert( dataOriginalResolution.IsNull() == false );

		//dataOriginalResolution = { 2604, 4096, 372 };

		auto t = Translate( -0.5, -0.5, -0.5 );
		auto s = Scale( Vec3f( dataOriginalResolution ) );
		auto spacing = Scale( Vec3f( 1, 1, 6 ) );
		trans = MakeRef<Transform>( spacing * s * t );

		std::ifstream in( filename.c_str() );
		if ( !in.good() ) {
			std::cout << "ERROR: loading obj:(" << filename << ") file is not good"
					  << "\n";
			exit( 0 );
		}
		std::vector<ysl::Point3f> point_set;
		std::vector<int> point_id;
		char buffer[ 256 ], str[ 255 ];
		float f1, f2, f3;
		while ( !in.getline( buffer, 255 ).eof() ) {
			buffer[ 255 ] = '\0';
			sscanf_s( buffer, "%s", str, 255 );
			int id1, id2;
			// reading a vertex
			if ( buffer[ 0 ] == 'v' && ( buffer[ 1 ] == ' ' || buffer[ 1 ] == 32 ) ) {
				if ( sscanf( buffer, "v %f %f %f", &f1, &f2, &f3 ) == 3 ) {
					point_set.push_back( { f1, f2, f3 } );
					// mesh.verts.push_back(float3(f1, f2, f3));
				}
			} else if ( sscanf( buffer, "l %d %d", &id1, &id2 ) == 2 ) {
				point_id.push_back( id1 - 1 );
				point_id.push_back( id2 - 1 );

			} else {
				sscanf( buffer, "g %s", str );
				//std::cout << "ERROR: vertex not in wanted format in OBJLoader" << "\n";
				//exit( -1 );
			}
		}

		std::cout
		  << "Obj file : " << filename << " has been loaded." << std::endl;
		std::cout << "Point number : " << point_set.size() << std::endl;
		std::cout << "Point id : " << point_id.size() << std::endl;
		for ( auto &vv : point_set ) {
			vv.x /= dataOriginalResolution.x;
			vv.y /= dataOriginalResolution.y;
			vv.z /= dataOriginalResolution.z;
		}

		const auto vertexArray = MakeRef<ArrayFloat3>();
		vertexArray->GetBufferObject()->SetLocalData( point_set.data(), point_set.size() * 3 * sizeof( float ) );
		//vertexArray->SetbufferObjectDataDirty( true );
		const auto indexArray = MakeRef<ArrayUInt>();
		indexArray->GetBufferObject()->SetLocalData( point_id.data(), point_id.size() * sizeof( int ) );
		//indexArray->SetbufferObjectDataDirty( true );

		//neroActorCallback->SetVertexArray( vertexArray, indexArray );
		Update( vertexArray, indexArray );
		//Update( MakeRef<ArrayFloat3>(), MakeRef<ArrayUInt>() );
	}
	void OnActorRenderStartedEvent( Actor *actor, const Camera *camera, Renderable *renderable, const Shading *shading, int pass ) override
	{
		assert( pass == 0 );
		(void)pass;
		if ( shading ) {
			//auto const program = shading->GetProgram().get();
			//const auto eyePos = camera->GetPosition();
			//const auto eye_position = program->GetGenericUniformLocation( "viewPos" );
			//if ( eye_position != -1 )
			//	actor->CreateGetUniformSet()->CreateGetUniform( "viewPos" )->SetUniform3f( 1, eyePos.ConstData() );
		}
	}
	void BindToActor( Ref<Actor> actor )
	{
		if ( actor ) {
			const auto shared_this = std::static_pointer_cast<NeroActorEventCallback>( shared_from_this() );
			actor->RemoveActorRenderEventCallback( shared_this );
			actor->AddActorRenderEventCallback( shared_this );
			actor->SetRenderable( neroGeometry, 0 );
			actor->SetTransform( trans );
		}
	}
	Ref<Primitive> ProxyGeometry() { return neroGeometry; }

private:
	void Update( Ref<ArrayFloat3> vertex, Ref<ArrayUInt> index )
	{
		assert( neroGeometry );
		if ( neroGeometry == nullptr ) {
			std::cout << "asdfsdafsaf";
			return;
		}
		//texCoordArray->GetBufferObject()->SetLocalData( texCoords, sizeof( texCoords ) );
		neroGeometry->SetVertexPositionArray( vertex );
		neroGeometry->SetVertexTexCoordArray( vertex );
		auto drawCall = MakeRef<DrawElementsUInt>();
		drawCall->SetIndexBuffer( index );
		drawCall->SetPrimitiveType( PrimitiveType::PT_LINES );
		drawCall->SetLineWidth( 4 );
		//neroGeometry->DrawCalls().push_back( drawCall );
		neroGeometry->DrawCalls().clear();
		neroGeometry->DrawCalls().push_back( drawCall );
		//drawCall->UpdateDirtyBufferObject( BufferObjectUpdateMode::VM_UM_KeepRAM );

		std::cout << "Draw Call Count:" << neroGeometry->DrawCalls().size() << " " << drawCall->GetCount();
	}
	Ref<Primitive> neroGeometry;
	Ref<Transform> trans;
};

class VMAPPS_EXPORT_IMPORT VM_LargeVolumeRayCast : public VisualMan
{
public:
	VM_LargeVolumeRayCast() = default;
	VM_LargeVolumeRayCast( bool offline, const std::string &outFileName, const std::string &jsonFileName, const std::string &tfFilename );
	void InitEvent() override;
	void UpdateScene() override;
	void DestroyEvent() override;
	void MouseWheelEvent( int ydegree, int xdegree ) override;
	void FileDropEvent( const std::vector<std::string> &fileNames ) override;
	void ResizeEvent( int w, int h ) override;
	void KeyPressEvent( KeyButton key ) override;
	void MouseMoveEvent( MouseButton button, int xpos, int ypos ) override;
	void UpdateEvent() override;

private:
	void PrintInfo();
	void SetupShading();
	void SetupResources( const std::string &fileName );
	void SetupNero( const std::string &fileName );
	void SetupTF( const std::string &fileName );
	void SetupCamera( const std::string &fileName );
	void SetupConfigurationFiles( const std::vector<std::string> &fileNames );

	//bool offlineRendering = false;

	std::string outFileName;
	std::string jsonFile;
	std::string tfFunctionFile;

	LVDJSONStruct lvdJSON;

	Ref<Shading> rayCastShading;
	Ref<Aggregate> mrtAgt, raycastAgt, navigationAgt;
	Ref<OutOfCorePrimitive> oocPrimitive;
	Ref<Texture> intermediateResult;
	Ref<Texture> randomOffsets;
	Vec2i vSize;
	Ref<Transform> proxyGemoryScaleTrans;
	Ref<Transform> neroScaleTrans;
	Ref<Transform> navigationScale;
	Ref<ViewMatrixWrapper> navigationCameraViewMatrix;

	Ref<Actor> boundingboxActor;
	Ref<NeroActorEventCallback> neroActorCallback;
	Vec3i dataOriginalResolution;
	Vec3i dataResolution;
	
	Ref<TrivialSceneManager> sceneManager;
	Ref<Artist> neroArtist;
	Vec3i padding;
};
}  // namespace vm
}  // namespace ysl

#endif

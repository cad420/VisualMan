
#include "vm_largevolumeraycast.h"
#include "oocactorcallback.h"

#include <rendercontext.h>
#include <screenactor.h>
#include <trivialscenemanager.h>
#include <VMat/transformation.h>
#include <VMUtils/log.hpp>
#include <VMGraphics/objreader.h>
#include "blitframebuffer.h"
#include "actoreventcallback.h"
#include <framebuffer.h>
#include "drawelements.h"
#include <VMUtils/log.hpp>
namespace ysl
{
namespace vm
{
//using namespace ::vm;
FrustumEventCallback::FrustumEventCallback( Ref<Camera> camera ) :
  camera( camera )
{
	frustum = MakeRef<Primitive>();
	if ( camera ) {
		const auto vertexIndex = MakeRef<ArrayUInt>();
		unsigned int indices[ 24 ] = { 0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7 };
		vertexIndex->GetBufferObject()->SetLocalData( indices, sizeof( indices ) );
		vertexIndex->GetBufferObject()->ReallocBufferData( sizeof( indices ), indices, BU_STATIC_DRAW );
		vertexIndex->SetbufferObjectDataDirty( false );
		auto drawCall = MakeRef<DrawElementsUInt>();
		drawCall->SetPrimitiveType( PT_LINES );
		drawCall->SetIndexBuffer( vertexIndex );
		drawCall->SetLineWidth( 2.f );
		const auto vertexPositionArray = MakeRef<ArrayFloat3>();
		frustum->SetVertexPositionArray( vertexPositionArray );
		frustum->DrawCalls().push_back( drawCall );
		UpdateFrustum();
	}
}

void FrustumEventCallback::BindFrustumActor( Ref<Actor> actor )
{
	if ( actor ) {
		auto shared_this = std::static_pointer_cast<FrustumEventCallback>( shared_from_this() );
		actor->RemoveActorRenderEventCallback( shared_this );
		actor->AddActorRenderEventCallback( shared_this );
		actor->SetRenderable( frustum, 0 );
	}
}

void FrustumEventCallback::OnActorRenderStartedEvent( Actor *actor,
													  const Camera *camera,
													  Renderable *renderable,
													  const Shading *shading,
													  int pass )
{
	UpdateFrustum();
}

Ref<Primitive> FrustumEventCallback::GetFrustumPrimitive()
{
	return frustum;
}

void FrustumEventCallback::UpdateFrustum()
{
	const auto lines = camera->GetFrustumLines();
	frustum->GetVertexArray()->GetBufferObject()->SetLocalData( lines.data(), sizeof( Point3f ) * lines.size() );
	frustum->GetVertexArray()->SetbufferObjectDataDirty( true );
	frustum->SetBufferObjectDirty( true );
}

BoundingBoxEventCallback::BoundingBoxEventCallback()
{
	boundingBox = MakeCubeLines( Bound3f{ { 0, 0, 0 }, { 1, 1, 1 } } );
	boundingBox->DrawCalls().at( 0 )->SetLineWidth( 3.f );
}

void BoundingBoxEventCallback::BindBoundingBoxActor( Ref<Actor> actor )
{
	if ( actor ) {
		auto shared_this = std::static_pointer_cast<BoundingBoxEventCallback>( shared_from_this() );
		actor->RemoveActorRenderEventCallback( shared_this );
		actor->AddActorRenderEventCallback( shared_this );
		actor->SetRenderable( boundingBox, 0 );
	}
}

void BoundingBoxEventCallback::OnActorRenderStartedEvent( Actor *actor, const Camera *camera,
														  Renderable *renderable, const Shading *shading, int pass )
{
}

VM_LargeVolumeRayCast::VM_LargeVolumeRayCast( bool offline, const std::string &outFileName,
											  const std::string &jsonFileName, const std::string &tfFilename ) :
  outFileName( outFileName ),
  jsonFile( jsonFileName ),
  tfFunctionFile( tfFilename )
{
}

void VM_LargeVolumeRayCast::InitEvent()

{
	PrintInfo();
	// Pipline Configuration

	/*[0] Resources Initialization****************************************
			 *
			 * entryTexture:
			 * exitTexture:
			 * intermediateResult:
			 *
			 */

	//
	//Context()->SetAutomaticallyUpdate(false);

	const auto w = Context()->GetFramebuffer()->Width();
	const auto h = Context()->GetFramebuffer()->Height();
	vSize = Vec2i{ w, h };

	auto texParam = MakeRef<TexCreateParams>();
	texParam->SetSize( vSize.x, vSize.y, 0 );
	texParam->SetTextureFormat( TF_RGBA32F );
	texParam->SetTextureTarget( TD_TEXTURE_2D );
	// entry texture
	auto entryTexture = MakeRef<Texture>();
	entryTexture->SetSetupParams( texParam );
	entryTexture->CreateTexture();	// needless
	// exit texture
	auto exitTexture = MakeRef<Texture>();
	exitTexture->SetSetupParams( texParam );
	exitTexture->CreateTexture();
	// intermediate result texture
	intermediateResult = MakeRef<Texture>();
	intermediateResult->SetSetupParams( texParam );
	intermediateResult->CreateTexture();

	//Debug hxy
	// nueral texture
	auto neuralTexture = MakeRef<Texture>();
	neuralTexture->SetSetupParams( texParam );
	neuralTexture->CreateTexture();

	/*[1]Multi Render Targets Aggregation*****************************************
			 *
			 * This aggregate is used to the render the front and back face of the proxy
			 * geometry of the volume into multi render targets. So the the user should
			 * control this camera to interact.
			 *****************************************************************************/

	mrtAgt = MakeRef<Aggregate>();
	//auto depthAttachment = MakeRef<vm::FBODepthAttachment>(DepthBufferFormat::DBF_DEPTH_COMPONENT24);
	auto mrtFBO = Context()->CreateFramebufferObject( vSize.x, vSize.y, RDB_COLOR_ATTACHMENT0, RDB_COLOR_ATTACHMENT0 );
	mrtFBO->AddTextureAttachment( AP_COLOR_ATTACHMENT0, MakeRef<FBOTextureAttachment>( entryTexture ) );
	mrtFBO->AddTextureAttachment( AP_COLOR_ATTACHMENT1, MakeRef<FBOTextureAttachment>( exitTexture ) );
	//Debug hxy
	mrtFBO->AddTextureAttachment( AP_COLOR_ATTACHMENT2, MakeRef<FBOTextureAttachment>( neuralTexture ) );
	mrtFBO->AddDepthAttachment( MakeRef<FBODepthAttachment>( DBF_DEPTH_COMPONENT24 ) );

	//mrtFBO->AddDepthAttachment(depthAttachment);
	//Debug hxy
	//mrtFBO->SetDrawBuffers( RDB_COLOR_ATTACHMENT0, RDB_COLOR_ATTACHMENT1 );
	mrtFBO->SetDrawBuffers( RDB_COLOR_ATTACHMENT0, RDB_COLOR_ATTACHMENT1, RDB_COLOR_ATTACHMENT2 );
	mrtAgt->Renderers().at( 0 )->SetFramebuffer( mrtFBO );
	BindCameraEvent( mrtAgt->CreateGetCamera() );

	auto positionShading = MakeRef<Shading>();
	positionShading->CreateGetEnableStateSet()->Enable( EN_BLEND );
	positionShading->CreateGetEnableStateSet()->Disable( EN_DEPTH_TEST );
	positionShading->CreateGetRenderStateSet()->SetRenderState( MakeRef<BlendFuncState>( BF_ONE, BF_ONE, BF_ONE, BF_ONE ), -1 );  // Think why
	positionShading->CreateGetRenderStateSet()->SetRenderState( MakeRef<FrontFaceState>( FF_CW ), -1 );

	auto positionGLSL = positionShading->CreateGetProgram();
	auto fragShader = MakeRef<GLSLFragmentShader>();
	fragShader->SetFromFile( R"(glsl/position_f.glsl)" );
	auto vertShader = MakeRef<GLSLVertexShader>();
	vertShader->SetFromFile( R"(glsl/position_v.glsl)" );
	positionGLSL->AttachShader( fragShader );
	positionGLSL->AttachShader( vertShader );
	positionGLSL->BindFragDataLocation( 0, "entryPos" );  // MRTs
	positionGLSL->BindFragDataLocation( 1, "exitPos" );
	//Debug hxy

	auto geometryArtist = MakeRef<Artist>();
	geometryArtist->GetLOD( 0 )->push_back( positionShading );
	auto t = Translate( -0.5, -0.5, -0.5 );
	auto s = Scale( 1, 1, 1 );

	proxyGemoryScaleTrans = MakeRef<Transform>( s * t );
	const auto geometryActor = MakeRef<Actor>( nullptr, geometryArtist, proxyGemoryScaleTrans );
	auto geometryActorCallback = MakeRef<RayCast2ActorEventCallback>();
	geometryActorCallback->BindToActor( geometryActor );

	auto neroShading = MakeRef<Shading>();
	/*auto st = MakeRef<PolygonModeState>();
	st->SetFrontMode( PM_LINE );
	st->SetBackMode( PM_LINE );
	neroShading->CreateGetRenderStateSet()->SetRenderState( st, -1 );*/
	neroShading->CreateGetEnableStateSet()->Enable( EN_DEPTH_TEST );
	
	auto neroGLSL = neroShading->CreateGetProgram();
	auto neroFragShader = MakeRef<GLSLFragmentShader>();
	neroFragShader->SetFromFile( R"(glsl/position_n_f.glsl)" );
	auto neroVertShader = MakeRef<GLSLVertexShader>();
	neroVertShader->SetFromFile( R"(glsl/position_n_v.glsl)" );
	neroGLSL->AttachShader( neroFragShader );
	neroGLSL->AttachShader( neroVertShader );
	neroGLSL->BindFragDataLocation( 2, "neuralPos" );

	neroArtist = MakeRef<Artist>();
	neroArtist->GetLOD( 0 )->push_back( neroShading );

	//auto t = Translate( -0.5, -0.5, -0.5 );
	//auto s = Scale( 1, 1, 1 );

	//neroScaleTrans = MakeRef<Transform>( s * t );

	sceneManager = MakeRef<TrivialSceneManager>();
	sceneManager->AddActor( geometryActor );
	//sceneManager->AddActor( neroActor );

	//
	mrtAgt->SceneManager().push_back( sceneManager );
	mrtAgt->CreateGetCamera()->GetViewport()->SetClearFlag( CF_CLEAR_COLOR_DEPTH);
	mrtAgt->CreateGetCamera()->GetViewport()->SetClearColor( Vec4f{ 0.f, 0.f, 0.f, 1.f } );
	mrtAgt->CreateGetCamera()->GetViewport()->SetClearDepth( 1 );

	// Thumbnail aggregation
	navigationAgt = MakeRef<Aggregate>();
	auto frustumShading = MakeRef<Shading>();
	frustumShading->CreateGetEnableStateSet()->Enable( EN_DEPTH_TEST );
	frustumShading->CreateGetUniformSet()->CreateGetUniform( "object_color" )->SetUniform4f( 1, Vec4f{ 0, 0, 1, 1 }.ConstData() );

	//navigationShading->CreateGetUniformSet()->CreateGetUniform("view_pos")->SetUniform3f(1, Vec3f{ 1,1,1 }.ConstData());

	auto navigationGLSL = frustumShading->CreateGetProgram();
	auto navigationVertShader = MakeRef<GLSLVertexShader>();
	navigationVertShader->SetFromFile( R"(glsl/trivial_no_illumination_vs.glsl)" );
	auto navigationFragShader = MakeRef<GLSLFragmentShader>();
	navigationFragShader->SetFromFile( R"(glsl/trivial_no_illumination_fs.glsl)" );
	navigationGLSL->AttachShader( navigationFragShader );
	navigationGLSL->AttachShader( navigationVertShader );

	auto frustumArtist = MakeRef<Artist>();
	frustumArtist->GetLOD( 0 )->push_back( frustumShading );
	auto frustumActor = MakeRef<Actor>( nullptr, frustumArtist, nullptr );
	auto frustumActorEventCallback = MakeRef<FrustumEventCallback>( mrtAgt->CreateGetCamera() );
	frustumActorEventCallback->BindFrustumActor( frustumActor );

	auto boundingboxShading = MakeRef<Shading>();
	boundingboxShading->CreateGetProgram()->AttachShader( navigationFragShader );
	boundingboxShading->CreateGetProgram()->AttachShader( navigationVertShader );
	boundingboxShading->CreateGetUniformSet()->CreateGetUniform( "object_color" )->SetUniform4f( 1, Vec4f{ 1, 0, 0, 1 }.ConstData() );

	auto boundingBoxArtist = MakeRef<Artist>();
	boundingBoxArtist->GetLOD( 0 )->push_back( boundingboxShading );

	navigationScale = MakeRef<Transform>();
	boundingboxActor = MakeRef<Actor>( nullptr, boundingBoxArtist, proxyGemoryScaleTrans );
	auto boundingboxEventCallback = MakeRef<BoundingBoxEventCallback>();
	boundingboxEventCallback->BindBoundingBoxActor( boundingboxActor );

	/*
			 * TODO:: Some memory crash will emerge when above variables are destructed.
			 * maybe there is something wrong when using smart pointer. such as circular reference
			 */

	auto navigationSceneManager = MakeRef<TrivialSceneManager>();
	navigationSceneManager->AddActor( frustumActor );
	navigationSceneManager->AddActor( boundingboxActor );
	navigationAgt->SceneManager().push_back( navigationSceneManager );
	navigationAgt->Renderers().at( 0 )->SetFramebuffer( Context()->GetFramebuffer() );
	navigationCameraViewMatrix = MakeRef<ViewMatrixWrapper>( Point3f{ 5, 5, 5 }, Vec3f{ 0, 1, 0 }, Point3f{ 0, 0, 0 } );
	navigationAgt->CreateGetCamera()->SetViewMatrixWrapper( navigationCameraViewMatrix );
	navigationAgt->CreateGetCamera()->GetViewport()->SetClearFlag( CF_CLEAR_COLOR_DEPTH );
	navigationAgt->CreateGetCamera()->GetViewport()->SetClearColor( Vec4f{ 1.f, 1.f, 1.f, 1.f } );
	navigationAgt->CreateGetCamera()->GetViewport()->SetPosition( 0, 0 );
	navigationAgt->CreateGetCamera()->SetNearPlane( 1.0 );
	navigationAgt->CreateGetCamera()->SetNearPlane( 20000.f );
	navigationAgt->CreateGetCamera()->GetViewport()->SetEnableScissor( true );

	/*[2] Ray Cast Aggregation ********************************************************
			 *
			 * This aggregation is used to execute the ray casting volume rendering.
			 * Intermediate result is saved in a texture between the intervals of
			 * resources swapping.
			 *********************************************************************************/

	raycastAgt = MakeRef<Aggregate>();
	auto intermediateFBO = Context()->CreateFramebufferObject( vSize.x, vSize.y, RDB_COLOR_ATTACHMENT0, RDB_COLOR_ATTACHMENT0 );
	intermediateFBO->AddTextureAttachment( AP_COLOR_ATTACHMENT0, MakeRef<FBOTextureAttachment>( intermediateResult ) );
	intermediateFBO->SetDrawBuffers( RDB_COLOR_ATTACHMENT0 );
	//intermediateFBO->AddDepthAttachment(depthAttachment);
	intermediateFBO->CheckFramebufferStatus();

	rayCastShading = MakeRef<Shading>();
	auto raycastGLSL = rayCastShading->CreateGetProgram();
	auto fs = MakeRef<GLSLFragmentShader>();
	fs->SetFromFile( R"(glsl/blockraycasting_f.glsl)" );
	auto vs = MakeRef<GLSLVertexShader>();
	vs->SetFromFile( R"(glsl/raycast_vs.glsl)" );
	raycastGLSL->AttachShader( fs );
	raycastGLSL->AttachShader( vs );
	rayCastShading->CreateGetTextureImageUnit( 4 )->SetTexture( intermediateResult );
	rayCastShading->CreateGetTextureImageUnit( 2 )->SetTexture( entryTexture );
	rayCastShading->CreateGetTextureImageUnit( 3 )->SetTexture( exitTexture );
	//Debug hxy
	rayCastShading->CreateGetTextureImageUnit( 5 )->SetTexture( neuralTexture );

	rayCastShading->CreateGetUniformSet()->CreateGetUniform( "texTransfunc" )->SetUniformValue( 4 );
	rayCastShading->CreateGetUniformSet()->CreateGetUniform( "cacheVolume0" )->SetUniformValue( 1 );
	rayCastShading->CreateGetUniformSet()->CreateGetUniform( "cacheVolume1" )->SetUniformValue( 2 );
	rayCastShading->CreateGetUniformSet()->CreateGetUniform( "cacheVolume2" )->SetUniformValue( 3 );
	rayCastShading->CreateGetUniformSet()->CreateGetUniform( "step" )->SetUniformValue( 0.01f );
	rayCastShading->CreateGetUniformSet()->CreateGetUniform( "ka" )->SetUniformValue( 1.0f );
	rayCastShading->CreateGetUniformSet()->CreateGetUniform( "kd" )->SetUniformValue( 1.0f );
	rayCastShading->CreateGetUniformSet()->CreateGetUniform( "ks" )->SetUniformValue( 50.f );
	Vec3f lightDir{ 1.0f, 1.0f, 1.0f };
	rayCastShading->CreateGetUniformSet()->CreateGetUniform( "lightdir" )->SetUniform3f( 1, lightDir.Data() );
	rayCastShading->CreateGetUniformSet()->CreateGetUniform( "halfway" )->SetUniform3f( 1, lightDir.Data() );

	auto effect2 = MakeRef<Artist>();
	effect2->GetLOD( 0 )->push_back( rayCastShading );

	const auto screenActor = MakeRef<Actor>( nullptr, effect2, proxyGemoryScaleTrans );
	auto screenActorCallback = MakeRef<ScreenActorEventCallback>( mrtAgt->CreateGetCamera() );
	oocPrimitive = MakeRef<OutOfCorePrimitive>();
	screenActorCallback->SetPrimitive( oocPrimitive );	// Out Of Core
	//SetupResources("");
	//SetupTF("");

	screenActorCallback->BindToActor( screenActor );
	const auto sceneManager = MakeRef<TrivialSceneManager>();
	sceneManager->AddActor( screenActor );

	raycastAgt->SceneManager().push_back( sceneManager );
	raycastAgt->Renderers().at( 0 )->SetFramebuffer( intermediateFBO );
	auto rect = Bound2i{ { 0, 0 }, { vSize.x, vSize.y } };

	// This object will blit the final redering result into default framebuffer
	auto blit = MakeRef<BlitFramebufferEvent>( intermediateFBO, rect, Context()->GetFramebuffer(), rect, BufferBits::VM_BB_COLOR_BUFFER_BIT );
	raycastAgt->Renderers().at( 0 )->AddRenderFinishedEventCallback( blit );
	//raycastAgt->SetCamera(camera);
	raycastAgt->CreateGetCamera()->SetViewMatrixWrapper( mrtAgt->CreateGetCamera()->GetViewMatrixWrapper() );
	raycastAgt->CreateGetCamera()->SetPerspectiveMatrix( mrtAgt->CreateGetCamera()->GetPerspectiveMatrix() );
	raycastAgt->CreateGetCamera()->GetViewport()->SetClearFlag( CF_CLEAR_COLOR );
	raycastAgt->CreateGetCamera()->GetViewport()->SetClearColor( Vec4f{ 0.f, 0.f, 0.f, 0.f } );

	auto outOfCoreAgts = MakeRef<SerializedAggregates>();
	SetAggregation( outOfCoreAgts );
	outOfCoreAgts->GetAggregates().push_back( mrtAgt );

	outOfCoreAgts->GetAggregates().push_back( raycastAgt );

	//outOfCoreAgts->GetAggregates().push_back(navigationAgt);

	SetupConfigurationFiles( { jsonFile, tfFunctionFile } );
}

void VM_LargeVolumeRayCast::UpdateScene()
{
	//const auto cam = mrtAgt->CreateGetCamera();
	//cam->GetViewMatrixWrapper()->RotateCamera(cam->GetUp(), 10);
	//cam->Movement(cam->GetFront().Normalized(),100);
	//cam->SetFov(int(cam->GetFov())-2);
}

void VM_LargeVolumeRayCast::DestroyEvent()
{
	rayCastShading.reset();
	mrtAgt = nullptr;
	raycastAgt = nullptr;
	oocPrimitive.reset();
	proxyGemoryScaleTrans = nullptr;
	intermediateResult = nullptr;
	VisualMan::DestroyEvent();
}

void VM_LargeVolumeRayCast::MouseWheelEvent( int ydegree, int xdegree )
{
	const auto fov = mrtAgt->CreateGetCamera()->GetFov();
	mrtAgt->CreateGetCamera()->SetFov( fov + ydegree );
	raycastAgt->CreateGetCamera()->SetFov( fov + ydegree );
	raycastAgt->CreateGetCamera()->SetNearPlane( mrtAgt->CreateGetCamera()->GetNearPlane() );
	raycastAgt->CreateGetCamera()->SetFarPlane( mrtAgt->CreateGetCamera()->GetFarPlane() );
	raycastAgt->CreateGetCamera()->SetAspectRation( mrtAgt->CreateGetCamera()->GetAspectRatio() );
	Context()->Update();
}

void VM_LargeVolumeRayCast::FileDropEvent( const std::vector<std::string> &fileNames )
{
	SetupConfigurationFiles( fileNames );
	//Context()->Update();
}

void VM_LargeVolumeRayCast::ResizeEvent( int w, int h )
{
	VisualMan::ResizeEvent( w, h );
	if ( raycastAgt ) {
		raycastAgt->CreateGetCamera()->GetViewport()->SetWidth( w );  // only for clear use
		raycastAgt->CreateGetCamera()->GetViewport()->SetHeight( h );
	}
	if ( mrtAgt ) {
		mrtAgt->CreateGetCamera()->GetViewport()->SetWidth( w );
		mrtAgt->CreateGetCamera()->GetViewport()->SetHeight( h );
	}
	navigationAgt->CreateGetCamera()->GetViewport()->SetViewportSize( w * 3.0 / 10, h * 3.0 / 10 );
	//Context()->Update();
}

void VM_LargeVolumeRayCast::KeyPressEvent( KeyButton key )
{
	// Save Camera
	float sensity = 10;
	if ( key == KeyButton::Key_C ) {
		SaveCameraAsJson( mrtAgt->CreateGetCamera(), "vmCamera.cam" );
	} else if ( key == KeyButton::Key_R ) {
		mrtAgt->CreateGetCamera()->GetViewMatrixWrapper()->SetPosition( Point3f{ 0, 0, 0 } );
		mrtAgt->CreateGetCamera()->SetFov( 60 );
		mrtAgt->CreateGetCamera()->GetViewMatrixWrapper()->SetCenter( Point3f{ 1000, 1000, 1000 } );
	} else if ( key == KeyButton::Key_F ) {
		manipulator->SetFPSCamera( !manipulator->IsFPSCamera() );
	}
}

void VM_LargeVolumeRayCast::MouseMoveEvent( MouseButton button, int xpos, int ypos )
{
	VisualMan::MouseMoveEvent( button, xpos, ypos );
	//std::cout << mrtAgt->CreateGetCamera()->GetPosition() << std::endl;
	Context()->Update();
}

void VM_LargeVolumeRayCast::UpdateEvent()
{
	VisualMan::UpdateEvent();

	std::string title = "LVD Render -- fps:" + std::to_string( GetFPS() );
	Context()->SetWindowTitle( title );
	if ( outFileName.empty() == false ) {
		intermediateResult->SaveTextureAs( "D:\\Desktop\\res.png" );
		Context()->Terminate();
	}
}

void VM_LargeVolumeRayCast::PrintInfo()
{
	std::cout << "---------------Demo Info-------------------------------------------\n";
	std::cout << "C to save camera at current directories\n";
	std::cout << "Drag the .lvd file to the window to open the data file\n";
	std::cout << "Drag the .tf file to the window to open the transfer function file\n";
	std::cout << "Drag the .json file to the window to config\n";
	std::cout << "---------------Demo Info-------------------------------------------\n";
}

void VM_LargeVolumeRayCast::SetupShading()
{
}

void VM_LargeVolumeRayCast::SetupResources( const std::string &fileName )
{
	assert( oocPrimitive );
	assert( rayCastShading );
	Ref<OutOfCoreVolumeTexture> oocResources;

	try {
		std::ifstream json( fileName );
		json >> lvdJSON;
		LVDFileInfo lvdInfo;
		lvdInfo.fileNames = lvdJSON.fileNames;
		lvdInfo.samplingRate = lvdJSON.samplingRate;
		lvdInfo.spacing = { lvdJSON.spacing[ 0 ], lvdJSON.spacing[ 1 ], lvdJSON.spacing[ 2 ] };

		oocResources = MakeRef<OutOfCoreVolumeTexture>( lvdInfo, Context()->GetDeviceTotalMemorySize() );
	} catch ( std::runtime_error &e ) {
		::vm::Warning( "{}", e.what() );
		return;
	}

	for ( int i = 0; i < oocResources->GetTextureUnitCount(); i++ ) {
		rayCastShading->CreateGetTextureSampler( i + 1 )->SetTexture( oocResources->GetVolumeTexture( i ) );
	}

	//rayCastShading->CreateGetTextureSampler(1)->SetTexture(oocResources->GetVolumeTexture(0));
	//rayCastShading->CreateGetTextureSampler(2)->SetTexture(oocResources->GetVolumeTexture(1));
	//rayCastShading->CreateGetTextureSampler(3)->SetTexture(oocResources->GetVolumeTexture(2));
	rayCastShading->CreateGetAtomicCounter( 3 )->SetBufferObject( oocResources->GetAtomicCounterBuffer() );
	rayCastShading->CreateGetSSBO( 2 )->SetBufferObject( oocResources->GetPageTableBuffer() );
	rayCastShading->CreateGetSSBO( 0 )->SetBufferObject( oocResources->GetHashBuffer() );
	rayCastShading->CreateGetSSBO( 1 )->SetBufferObject( oocResources->GetBlockIDBuffer() );
	rayCastShading->CreateGetSSBO( 3 )->SetBufferObject( oocResources->GetLODInfoBuffer() );

	auto v = oocResources->Padding();
	rayCastShading->CreateGetUniformSet()->CreateGetUniform( "repeatOffset" )->SetUniform3i( 1, v.Data() );
	rayCastShading->CreateGetUniformSet()->CreateGetUniform( "lodNum" )->SetUniformValue( oocResources->GetLODCount() );
	oocPrimitive->SetOutOfCoreResources( oocResources );

	int lodCount = oocResources->GetLODCount();
	dataOriginalResolution = oocResources->DataResolutionWithPadding( 0 );
	dataResolution = oocResources->DataResolution( 0 );
	padding = oocResources->Padding();
	
	auto t = Translate( -0.5, -0.5, -0.5 );
	//auto s = Scale( Vec3f( dataOriginalResolution ) );
	auto s = Scale( Vec3f( dataResolution ) );
	auto spacing = Scale( Vec3f( 1, 1, 6 ) );
	*proxyGemoryScaleTrans = Transform( spacing * s * t );
	//*navigationCameraViewMatrix = ViewMatrixWrapper( Point3f( MinComponent( dataOriginalResolution ), MinComponent( dataOriginalResolution ), MaxComponent( dataOriginalResolution * 2 ) ), Vec3f{ 0, 1, 0 }, Point3f{ 0, 0, 0 } );
	*navigationCameraViewMatrix = ViewMatrixWrapper( Point3f( MinComponent( dataResolution ), 
		MinComponent( dataResolution ), MaxComponent( dataResolution * 2 ) ),
													 Vec3f{ 0, 1, 0 }, Point3f{ 0, 0, 0 } );
	Context()->Update();

	
}

void VM_LargeVolumeRayCast::SetupNero( const std::string &filename )
{
	// reading obj

	//ysl::Vec3i data_volume = dataResolution + padding + padding;
	ysl::Vec3i data_volume = dataResolution;
	//data_volume.z += 4 * padding.z;

	const auto neroActor = MakeRef<Actor>( nullptr, neroArtist, nullptr );
	neroActorCallback = MakeRef<NeroActorEventCallback>( filename, data_volume );

	std::cout << "nerual data resolution : [" << data_volume.x << ","
			  << data_volume.y << "," << data_volume.z << "]" << std::endl;

	neroActorCallback->BindToActor( neroActor );
	sceneManager->AddActor( neroActor );
}

void VM_LargeVolumeRayCast::SetupTF( const std::string &fileName )
{
	assert( rayCastShading );
	try {
		const auto tfTex = MakeTransferFunction1DTexture( fileName );

		const auto preTex = MakePreIntegratedTransferFunction2DTexture( fileName );

		rayCastShading->CreateGetTextureSampler( 4 )->SetTexture( tfTex );
		rayCastShading->CreateGetTextureSampler( 5 )->SetTexture( preTex );
	} catch ( std::runtime_error &e ) {
		::vm::Warning( "Can not load .tf file" );
		return;
	}
	Context()->Update();
}

void VM_LargeVolumeRayCast::SetupCamera( const std::string &fileName )
{
	//const auto camera = CreateCamera(fileName);
	ConfigCamera( mrtAgt->CreateGetCamera().get(), fileName );
	std::cout << std::endl
			  << mrtAgt->CreateGetCamera()->GetViewMatrixWrapper()->GetViewMatrix().Matrix() << std::endl;
	Context()->Update();
}

void VM_LargeVolumeRayCast::SetupConfigurationFiles( const std::vector<std::string> &fileNames )
{
	for ( const auto &each : fileNames ) {
		if ( each.empty() )
			continue;
		auto extension = each.substr( each.find_last_of( '.' ) );
		bool found = false;
		if ( extension == ".tf" ) {
			SetupTF( each );
			found = true;
		} else if ( extension == ".lods" ) {
			SetupResources( each );
			found = true;
		} else if ( extension == ".cam" ) {
			SetupCamera( each );
			found = true;
		} else if ( extension == ".obj" ) {
			SetupNero( each );
			found = true;
		}
		if ( found )
			break;
	}
}
}  // namespace vm
}  // namespace ysl

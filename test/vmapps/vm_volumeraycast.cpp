
#include "vm_volumeraycast.h"
#include "rendercontext.h"
#include "shaderprogram.h"
#include "texture.h"
#include "trivialscenemanager.h"
#include "drawelements.h"
#include "blitframebuffer.h"
#include "actoreventcallback.h"
#include "primitive.h"

namespace vm
{
void VM_VolumeRayCast::UpdateScene()
{
}

void VM_VolumeRayCast::InitEvent()
{
	//
	auto rayCastShading = MakeVMRef<Shading>();
	auto glsl = rayCastShading->CreateGetProgram();
	auto fragShader = MakeVMRef<GLSLFragmentShader>();
	fragShader->SetFromFile( R"(D:\code\MRE\resource\glsl\singlepassraycast_fs.glsl)" );
	auto vertShader = MakeVMRef<GLSLVertexShader>();
	vertShader->SetFromFile( R"(D:\code\MRE\resource\glsl\singlepassraycast_vs.glsl)" );
	glsl->AttachShader( fragShader );
	glsl->AttachShader( vertShader );
	// Read volume data
	Size3 volSize{ 480, 720, 120 };
	auto volumeTex = MakeVolumeTexture( R"(D:\scidata\combustion\mixfrac.raw)", volSize.x, volSize.y, volSize.z );
	// Read transfer function
	auto tfTex = MakeTransferFunction1DTexture( R"(D:\scidata\elt_tf1d2.TF1D)" );

	auto effect = MakeVMRef<Artist>();
	effect->GetLOD( 0 )->push_back( rayCastShading );

	auto scale = MakeVMRef<Transform>();
	scale->SetScale( Vec3f{ 480, 720, 120 }.Normalized() );

	auto actor = MakeVMRef<Actor>( nullptr, effect, scale );
	// Set illumination parameters
	actor->CreateGetUniformSet()->CreateGetUniform( "step" )->SetUniformValue( 0.001f );
	actor->CreateGetUniformSet()->CreateGetUniform( "ka" )->SetUniformValue( 1.0f );
	actor->CreateGetUniformSet()->CreateGetUniform( "kd" )->SetUniformValue( 1.0f );
	actor->CreateGetUniformSet()->CreateGetUniform( "ks" )->SetUniformValue( 50.0f );
	actor->CreateGetUniformSet()->CreateGetUniform( "texTransfunc" )->SetUniformValue( 1 );
	actor->CreateGetUniformSet()->CreateGetUniform( "texVolume" )->SetUniformValue( 0 );
	//actor->CreateGetUniformSet()->CreateGetUniform("testImage")->SetUniformValue(0);
	// Set texture samplers
	rayCastShading->CreateGetTextureSampler( 0 )->SetTexture( volumeTex );
	//rayCastShading->CreateGetTextureImageUnit(0)->SetTexture(volumeTex);
	rayCastShading->CreateGetTextureSampler( 1 )->SetTexture( tfTex );

	rayCastShading->CreateGetEnableStateSet()->Enable( EN_CULL_FACE );
	rayCastShading->CreateGetRenderStateSet()->SetRenderState( MakeVMRef<CullFaceState>( PF_FRONT ), -1 );
	// Enable
	rayCastShading->CreateGetEnableStateSet()->Enable( EN_DEPTH_TEST );
	rayCastShading->CreateGetRenderStateSet()->SetRenderState( MakeVMRef<DepthFuncState>( FU_LESS ), -1 );
	rayCastShading->CreateGetEnableStateSet()->Enable( EN_BLEND );
	rayCastShading->CreateGetRenderStateSet()->SetRenderState( MakeVMRef<BlendFuncState>( BF_DST_ALPHA, BF_DST_ALPHA, BF_ONE_MINUS_DST_ALPHA, BF_ONE_MINUS_DST_ALPHA ), -1 );

	rayCastCallback = MakeVMRef<RayCastActorEventCallback>();
	rayCastCallback->BindToActor( actor );
	// Create a frame and a scene manager
	//aggregate = MakeRef<Aggregate>();
	SetAggregation( MakeVMRef<Aggregate>() );
	sceneManager = MakeVMRef<TrivialSceneManager>();
	sceneManager->AddActor( actor );
	std::static_pointer_cast<Aggregate>( GetAggregate() )->SceneManager().push_back( sceneManager );
	// Set Clear flags
	std::static_pointer_cast<Aggregate>( GetAggregate() )->CreateGetCamera()->GetViewport()->SetClearFlag( CF_CLEAR_COLOR_DEPTH );
	// Set camera under control
	//manipulator->SetCamera(aggregate->GetCamera());
	BindCameraEvent( std::static_pointer_cast<Aggregate>( GetAggregate() )->CreateGetCamera() );

	assert( Context() );

	// Create A FBO
	auto w = std::static_pointer_cast<Aggregate>( GetAggregate() )->CreateGetCamera()->GetViewport()->GetWidth();
	auto h = std::static_pointer_cast<Aggregate>( GetAggregate() )->CreateGetCamera()->GetViewport()->GetHeight();
	Size2 viewportSize( w, h );
	auto fbo = Context()->CreateFramebufferObject( w, h, RDB_COLOR_ATTACHMENT0, RDB_COLOR_ATTACHMENT0 );
	fbo->AddDepthStencilAttachment( MakeVMRef<FBODepthStencilAttachment>( DSBT_DEPTH24_STENCIL8 ) );

	// Create a texture by given params
	auto texture = MakeVMRef<Texture>();
	auto texCreateParams = MakeVMRef<TexCreateParams>();
	texCreateParams->SetSize( viewportSize.x, viewportSize.y, 0 );
	texCreateParams->SetTextureFormat( TF_RGBA32F );
	texCreateParams->SetTextureTarget( TD_TEXTURE_RECTANGLE );
	texture->SetSetupParams( texCreateParams );
	assert( texture->CreateTexture() );
	//fbo->AddTextureAttachment(AP_COLOR_ATTACHMENT0, MakeRef<FBOTextureAttachment>(texture));
	fbo->AddColorAttachment( AP_COLOR_ATTACHMENT0, MakeVMRef<FBOColorBufferAttachment>() );
	std::static_pointer_cast<Aggregate>( GetAggregate() )->Renderers()[ 0 ]->SetFramebuffer( fbo );
	//GetAggregate()->Renderers()[0]->SetFramebuffer(Context()->GetFramebuffer());

	auto blit = MakeVMRef<BlitFramebufferEvent>( fbo, Bound2i{ { 0, 0 }, { 800, 600 } }, Context()->GetFramebuffer(), Bound2i{ { 0, 0 }, { 800, 600 } }, VM_BB_COLOR_BUFFER_BIT );
	std::static_pointer_cast<Aggregate>( GetAggregate() )->Renderers()[ 0 ]->AddRenderFinishedEventCallback( blit );

	//AddBoundingBox();
}

void VM_VolumeRayCast::AddBoundingBox()
{
	auto vertShader = MakeVMRef<vm::GLSLVertexShader>();
	vertShader->SetFromFile( R"(D:\code\MRE\resource\glsl\trivial_vs.glsl)" );
	auto fragShader = MakeVMRef<vm::GLSLFragmentShader>();
	fragShader->SetFromFile( R"(D:\code\MRE\resource\glsl\trivial_fs.glsl)" );

	auto shading = MakeVMRef<vm::Shading>();

	shading->CreateGetProgram()->AttachShader( vertShader );
	shading->CreateGetProgram()->AttachShader( fragShader );
	shading->CreateGetRenderStateSet()->SetRenderState( MakeVMRef<PolygonModeState>( PM_LINE, PM_LINE ), -1 );
	shading->CreateGetEnableStateSet()->Enable( EN_CULL_FACE );
	shading->CreateGetRenderStateSet()->SetRenderState( MakeVMRef<CullFaceState>( PF_FRONT ), -1 );
	// add render state

	auto artist = MakeVMRef<vm::Artist>();
	artist->GetLOD( 0 )->push_back( shading );
	auto scale = MakeVMRef<Transform>();
	scale->SetScale( Vec3f{ 10, 10, 10 }.Normalized() );
	const auto actor1 = MakeVMRef<vm::Actor>( rayCastCallback->ProxyGeometry(), artist, scale );
	auto actorEvent = MakeVMRef<MarchingCubeActorCallback>();
	actor1->AddActorRenderEventCallback( actorEvent );

	//const auto actor2 = MakeRef<vm::Actor>(teapotPrimitive, artist, nullptr);

	Vec3f lightColor{ 1.0, 1.0, 1.0 };
	Vec3f lightPos{ 10, 10, 5.0 };
	Vec4f objectColor{ .4, 0.3, 0.8, 1 };
	actor1->CreateGetUniformSet()->CreateGetUniform( "light_pos" )->SetUniform3f( 1, lightPos.Data() );
	actor1->CreateGetUniformSet()->CreateGetUniform( "light_color" )->SetUniform3f( 1, lightColor.Data() );
	actor1->CreateGetUniformSet()->CreateGetUniform( "object_color" )->SetUniform4f( 1, objectColor.Data() );

	sceneManager->AddActor( actor1 );
}
}  // namespace vm

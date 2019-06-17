
#include "vm_largevolumeraycast.h"
#include "oocactorcallback.h"

#include <rendercontext.h>
#include <screenactor.h>
#include <trivialscenemanager.h>
#include "blitframebuffer.h"
#include "actoreventcallback.h"


namespace ysl
{
	namespace vm
	{
		void VM_LargeVolumeRayCast::InitEvent()
		{
			// Pipline Configuration

			/*[0] Resources Initialization****************************************
			 *
			 * entryTexture:
			 * exitTexture:
			 * intermediateResult:
			 */

			Vec2i vSize{ 800,600 };
			auto texParam = MakeRef<TexCreateParams>();
			texParam->SetSize(vSize.x, vSize.y, 0);
			texParam->SetTextureFormat(TF_RGBA32F);
			texParam->SetTextureTarget(TD_TEXTURE_2D);
			// entry texture
			auto entryTexture = MakeRef<Texture>();
			entryTexture->SetSetupParams(texParam);
			entryTexture->CreateTexture();	// needless
			// exit texture
			auto exitTexture = MakeRef<Texture>();
			exitTexture->SetSetupParams(texParam);
			exitTexture->CreateTexture();
			// intermediate result texture
			auto intermediateResult = MakeRef<Texture>();
			intermediateResult->SetSetupParams(texParam);
			intermediateResult->CreateTexture();


			/*[1]Multi Render Targets Aggregation*****************************************
			 *
			 * This aggregate is used to the render the front and back face of the proxy
			 * geometry of the volume into multi render targets. So the the user should
			 * control this camera to interact.
			 *****************************************************************************/

			auto mrtAgt = MakeRef<Aggregate>();
			auto mrtFBO = Context()->CreateFramebufferObject(vSize.x, vSize.y, RDB_COLOR_ATTACHMENT0, RDB_COLOR_ATTACHMENT0);
			mrtFBO->AddTextureAttachment(AP_COLOR_ATTACHMENT0, MakeRef<FBOTextureAttachment>(entryTexture));
			mrtFBO->AddTextureAttachment(AP_COLOR_ATTACHMENT1, MakeRef<FBOTextureAttachment>(exitTexture));
			mrtFBO->SetDrawBuffers(RDB_COLOR_ATTACHMENT0, RDB_COLOR_ATTACHMENT1);
			mrtAgt->Renderers().at(0)->SetFramebuffer(mrtFBO);
			BindCameraEvent(mrtAgt->CreateGetCamera());

			auto positionShading = MakeRef<Shading>();
			positionShading->CreateGetEnableStateSet()->Enable(EN_BLEND);
			positionShading->CreateGetRenderStateSet()->SetRenderState(MakeRef<BlendFuncState>(BF_ONE, BF_ONE, BF_ONE, BF_ONE), -1);			// Think why
			positionShading->CreateGetRenderStateSet()->SetRenderState(MakeRef<FrontFaceState>(FF_CW), -1);

			auto positionGLSL = positionShading->CreateGetProgram();
			auto fragShader = MakeRef<GLSLFragmentShader>();
			fragShader->SetFromFile(R"(glsl/position_f.glsl)");
			auto vertShader = MakeRef<GLSLVertexShader>();
			vertShader->SetFromFile(R"(glsl/position_v.glsl)");
			positionGLSL->AttachShader(fragShader);
			positionGLSL->AttachShader(vertShader);
			positionGLSL->BindFragDataLocation(0, "entryPos");		// MRTs
			positionGLSL->BindFragDataLocation(1, "exitPos");

			auto artist = MakeRef<Artist>();
			artist->GetLOD(0)->push_back(positionShading);

			//auto scale = MakeRef<Transform>();
			//scale->SetScale(Vec3f(160, 240, 40).Normalized());

			auto geometryActor = MakeRef<Actor>(nullptr, artist, nullptr);
			auto geometryActorCallback = MakeRef<RayCast2ActorEventCallback>();
			geometryActorCallback->BindToActor(geometryActor);

			auto sceneManager = MakeRef<TrivialSceneManager>();
			sceneManager->AddActor(geometryActor);
			mrtAgt->SceneManager().push_back(sceneManager);
			mrtAgt->CreateGetCamera()->GetViewport()->SetClearFlag(CF_CLEAR_COLOR_DEPTH);
			mrtAgt->CreateGetCamera()->GetViewport()->SetClearColor(Vec4f{ 0.f,0.f,0.f,1.f });

			/*[2] Ray Cast Aggregation ********************************************************
			 *
			 * This aggregation is used to execute the ray casting volume rendering.
			 * Intermediate result is saved in a texture between the intervals of
			 * resources swapping.
			 *********************************************************************************/
			auto raycastAgt = MakeRef<Aggregate>();
			auto intermediateFBO = Context()->CreateFramebufferObject(800, 600, RDB_COLOR_ATTACHMENT0, RDB_COLOR_ATTACHMENT0);
			intermediateFBO->AddTextureAttachment(AP_COLOR_ATTACHMENT0, MakeRef<FBOTextureAttachment>(intermediateResult));
			intermediateFBO->SetDrawBuffers(RDB_COLOR_ATTACHMENT0);
			intermediateFBO->CheckFramebufferStatus();
			Vec3i volSize{ 160,240,40 };
			auto volumeTex = MakeVolumeTexture(R"(data\mixfrac160x240x40.raw)", volSize.x, volSize.y, volSize.z);
			//auto tfTex = MakeTransferFunction1D(R"(D:\scidata\elt_tf1d2.TF1D)");
			auto tfTex = MakeTransferFunction1DTexture({ Color::transparent,Color::blue,Color::yellow ,Color::green});

			auto rayCastShading = MakeRef<Shading>();
			auto raycastGLSL = rayCastShading->CreateGetProgram();
			auto fs = MakeRef<GLSLFragmentShader>();
			fs->SetFromFile(R"(glsl\blockraycasting_f.glsl)");
			auto vs = MakeRef<GLSLVertexShader>();
			vs->SetFromFile(R"(glsl\raycast_vs.glsl)");
			raycastGLSL->AttachShader(fs);
			raycastGLSL->AttachShader(vs);

			auto oocResources = MakeRef<OutOfCoreVolumeTexture>(R"(C:\data\s1_480_480_480_2_64.lvd)");
			
			rayCastShading->CreateGetTextureSampler(1)->SetTexture(oocResources->GetVolumeTexture());
		
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("cacheVolume")->SetUniformValue(1);

			rayCastShading->CreateGetTextureSampler(2)->SetTexture(tfTex);
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("texTransfunc")->SetUniformValue(2);

			rayCastShading->CreateGetTextureSampler(3)->SetTexture(entryTexture);
			rayCastShading->CreateGetTextureImageUnit(2)->SetTexture(entryTexture);
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("texStartPos")->SetUniformValue(3);
		
			rayCastShading->CreateGetTextureSampler(4)->SetTexture(exitTexture);
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("texEndPos")->SetUniformValue(4);

			//rayCastShading->CreateGetTextureSampler(5)->SetTexture(oocResources->GetMappingTableTexture());
			rayCastShading->CreateGetTextureImageUnit(1)->SetTexture(oocResources->GetMappingTableTexture());

			//rayCastShading->CreateGetTextureSampler(6)->SetTexture(intermediateResult);
			rayCastShading->CreateGetTextureImageUnit(4)->SetTexture(intermediateResult);

			rayCastShading->CreateGetAtomicCounter(3)->SetBufferObject(oocResources->GetAtomicCounterBuffer());
			rayCastShading->CreateGetSSBO(0)->SetBufferObject(oocResources->GetHashBuffer());
			rayCastShading->CreateGetSSBO(1)->SetBufferObject(oocResources->GetBlockIDBuffer());

			rayCastShading->CreateGetUniformSet()->CreateGetUniform("step")->SetUniformValue(0.001f);
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("ka")->SetUniformValue(1.0f);
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("kd")->SetUniformValue(1.0f);
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("ks")->SetUniformValue(50.f);

		
			
		
		

			auto v = oocResources->VirtualBlockDim();
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("totalPageTableSize")->SetUniform3i(1,v.Data());
			v = oocResources->DataResolution();
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("volumeDataSizeNoRepeat")->SetUniform3i(1,v.Data());
			v = oocResources->BlockSize() - oocResources->Padding();
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("blockDataSizeNoRepeat")->SetUniform3i(1,v.Data());
			v = oocResources->Padding();
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("repeatOffset")->SetUniform3i(1,v.Data());
			//rayCastShading->CreateGetUniformSet()->CreateGetUniform("texIntermediateResult")->SetUniformValue(6);




			auto effect2 = MakeRef<Artist>();
			effect2->GetLOD(0)->push_back(rayCastShading);

			auto screenActor = MakeRef<Actor>(nullptr, effect2, nullptr);
			auto screenActorCallback = MakeRef<ScreenActorEventCallback>();
			auto oocPrimitive = MakeRef<OutOfCorePrimitive>();
			screenActorCallback->SetPrimitive(oocPrimitive);// Out Of Core
			//oocResources->BindToOutOfCorePrimitive(oocPrimitive);
			oocPrimitive->SetOutOfCoreResources(oocResources);

			screenActorCallback->BindToActor(screenActor);
			sceneManager = MakeRef<TrivialSceneManager>();
			sceneManager->AddActor(screenActor);
			raycastAgt->SceneManager().push_back(sceneManager);

			raycastAgt->Renderers().at(0)->SetFramebuffer(intermediateFBO);
			auto rect = Bound2i{ {0,0},{vSize.x,vSize.y} };

			auto blit = MakeRef<BlitFramebufferEvent>(intermediateFBO, rect, Context()->GetFramebuffer(), rect, BufferBits::VM_BB_COLOR_BUFFER_BIT);
			raycastAgt->Renderers().at(0)->AddRenderFinishedEventCallback(blit);
			raycastAgt->CreateGetCamera()->GetViewport()->SetClearFlag(CF_CLEAR_COLOR);
			raycastAgt->CreateGetCamera()->GetViewport()->SetClearColor(Vec4f{ 1.f,1.f,1.f,1.f });

			auto serializedAgts= MakeRef<SerializedAggregates>();
			SetAggregation(serializedAgts);

			serializedAgts->GetAggregates().push_back(mrtAgt);
			serializedAgts->GetAggregates().push_back(raycastAgt);
		}

		void VM_LargeVolumeRayCast::UpdateScene()
		{

		}

		void VM_LargeVolumeRayCast::SetupShading()
		{

		}

		void VM_LargeVolumeRayCast::SetupResources()
		{

		}
	}
}

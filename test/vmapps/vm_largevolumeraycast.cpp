
#include "vm_largevolumeraycast.h"

#include "rendercontext.h"
#include "blitframebuffer.h"
#include "actoreventcallback.h"
#include "oocactorcallback.h"
#include "trivialscenemanager.h"


namespace ysl
{
	namespace vm
	{
		void VM_LargeVolumeRayCast::InitEvent()
		{
			// Pipline Configuration


			/*[0]*************************************************************************
			 * Resources Initialization
			 * 
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
			exitTexture->CreateTexture();


			/*[1]***************************************************************************
			 * Multi Render Targets Aggregation
			 * 
			 * This aggregate is used to the render the front and back face of the proxy 
			 * geometry of the volume into multi render targets. So the the user should
			 * control this camera to interact.
			 *****************************************************************************/

			auto mrtAgt = MakeRef<Aggregate>();
			auto mrtFBO = Context()->CreateFramebufferObject(vSize.x, vSize.y, RDB_COLOR_ATTACHMENT0, RDB_COLOR_ATTACHMENT0);
			mrtFBO->AddTextureAttachment(AP_COLOR_ATTACHMENT0,MakeRef<FBOTextureAttachment>(entryTexture));
			mrtFBO->AddTextureAttachment(AP_COLOR_ATTACHMENT1,MakeRef<FBOTextureAttachment>(exitTexture));
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
			auto geometryActorCallback = MakeRef<OOCActorEvent>();
			geometryActorCallback->BindToActor(geometryActor);

			auto sceneManager = MakeRef<TrivialSceneManager>();
			sceneManager->AddActor(geometryActor);
			mrtAgt->SceneManager().push_back(sceneManager);
			mrtAgt->CreateGetCamera()->GetViewport()->SetClearFlag(CF_CLEAR_COLOR_DEPTH);
			mrtAgt->CreateGetCamera()->GetViewport()->SetClearColor(Vec4f{0.f,0.f,0.f,1.f});



			//auto blit = MakeRef<BlitFramebufferEvent>(mrtFBO, Bound2i{{0,0},{vSize.x,vSize.y}}, Context()->GetFramebuffer(),Bound2i{{0,0},{vSize.x,vSize.y}}, VM_BB_COLOR_BUFFER_BIT);



			/*[2] Ray Cast Aggregation *********************************************************
			 * 
			 * This aggregation is used to execute the ray casting volume rendering.
			 * Intermediate result is saved in a texture between the intervals of 
			 * resources swapping. 
			 *********************************************************************************/


			auto raycastAgt = MakeRef<Aggregate>();
			//auto intermediateFBO = Context()->CreateFramebufferObject(800, 600, RDB_COLOR_ATTACHMENT0, RDB_COLOR_ATTACHMENT0);
			//intermediateFBO->AddTextureAttachment(AP_COLOR_ATTACHMENT0, MakeRef<FBOTextureAttachment>(intermediateResult));



			//raycastAgt->Renderers().at(0)->SetFramebuffer();

			/*[3] Main Aggregation******************************************************** 
			 *
			 *
			 *****************************************************************************/
			auto mainAgt = MakeRef<Aggregate>();
			//mainAgt->Renderers().at(0)->SetFramebuffer(Context()->GetFramebuffer());
			//std::static_pointer_cast<Aggregate>(GetAggregate())->CreateGetCamera()->GetViewport()->SetViewportSize(vSize.x,vSize.y);


			/*[4] Pipline Configuration *************************************************/

			auto serializedAgts= MakeRef<SerializedAggregates>();
			SetAggregation(serializedAgts);

			serializedAgts->GetAggregates().push_back(mrtAgt);
			//serializedAgts->GetAggregates().push_back(raycastAgt);
			//serializedAgts->GetAggregates().push_back(mainAgt);

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

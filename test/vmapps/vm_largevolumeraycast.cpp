
#include "vm_largevolumeraycast.h"
#include "oocactorcallback.h"

#include <rendercontext.h>
#include <screenactor.h>
#include <trivialscenemanager.h>
#include "blitframebuffer.h"
#include "actoreventcallback.h"
#include <transformation.h>


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
			 *
			 */

			Context()->SetAutomaticallyUpdate(false);

			const auto w = Context()->GetFramebuffer()->Width();
			const auto h = Context()->GetFramebuffer()->Height();

			const Vec2i vSize{ w,h };
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

			mrtAgt = MakeRef<Aggregate>();
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
			auto t = Translate(-0.5, -0.5, -0.5);
			auto s = Scale(8, 8, 12);

			auto scale = MakeRef<Transform>(s*t);

			const auto geometryActor = MakeRef<Actor>(nullptr, artist, scale);
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


			raycastAgt = MakeRef<Aggregate>();
			auto intermediateFBO = Context()->CreateFramebufferObject(vSize.x, vSize.y, RDB_COLOR_ATTACHMENT0, RDB_COLOR_ATTACHMENT0);
			intermediateFBO->AddTextureAttachment(AP_COLOR_ATTACHMENT0, MakeRef<FBOTextureAttachment>(intermediateResult));
			intermediateFBO->SetDrawBuffers(RDB_COLOR_ATTACHMENT0);
			intermediateFBO->CheckFramebufferStatus();

			rayCastShading = MakeRef<Shading>();
			auto raycastGLSL = rayCastShading->CreateGetProgram();
			auto fs = MakeRef<GLSLFragmentShader>();
			fs->SetFromFile(R"(glsl\blockraycasting_f.glsl)");
			auto vs = MakeRef<GLSLVertexShader>();
			vs->SetFromFile(R"(glsl\raycast_vs.glsl)");
			raycastGLSL->AttachShader(fs);
			raycastGLSL->AttachShader(vs);
			rayCastShading->CreateGetTextureImageUnit(4)->SetTexture(intermediateResult);
			rayCastShading->CreateGetTextureImageUnit(2)->SetTexture(entryTexture);
			rayCastShading->CreateGetTextureImageUnit(3)->SetTexture(exitTexture);

			rayCastShading->CreateGetUniformSet()->CreateGetUniform("texTransfunc")->SetUniformValue(4);
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("cacheVolume0")->SetUniformValue(1);
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("cacheVolume1")->SetUniformValue(2);
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("cacheVolume2")->SetUniformValue(3);
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("step")->SetUniformValue(0.00001f);
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("ka")->SetUniformValue(1.0f);
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("kd")->SetUniformValue(1.0f);
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("ks")->SetUniformValue(50.f);
			Vec3f lightDir{ 1.0f,1.0f,1.0f };
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("lightdir")->SetUniform3f(1,lightDir.Data());
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("halfway")->SetUniform3f(1, lightDir.Data());

			auto effect2 = MakeRef<Artist>();
			effect2->GetLOD(0)->push_back(rayCastShading);

			const auto screenActor = MakeRef<Actor>(nullptr, effect2, nullptr);
			auto screenActorCallback = MakeRef<ScreenActorEventCallback>();
			oocPrimitive = MakeRef<OutOfCorePrimitive>();
			screenActorCallback->SetPrimitive(oocPrimitive);// Out Of Core

			//SetupResources("");
			//SetupTF("");

			screenActorCallback->BindToActor(screenActor);
			sceneManager = MakeRef<TrivialSceneManager>();
			sceneManager->AddActor(screenActor);

			raycastAgt->SceneManager().push_back(sceneManager);
			raycastAgt->Renderers().at(0)->SetFramebuffer(intermediateFBO);
			auto rect = Bound2i{ {0,0},{vSize.x,vSize.y} };

			// This object will blit the final redering result into default framebuffer
			auto blit = MakeRef<BlitFramebufferEvent>(intermediateFBO, rect, Context()->GetFramebuffer(), rect, BufferBits::VM_BB_COLOR_BUFFER_BIT);
			raycastAgt->Renderers().at(0)->AddRenderFinishedEventCallback(blit);
			raycastAgt->CreateGetCamera()->GetViewport()->SetClearFlag(CF_CLEAR_COLOR);
			raycastAgt->CreateGetCamera()->GetViewport()->SetClearColor(Vec4f{ 0.f,0.f,0.f,0.f });

			auto outOfCoreAgts = MakeRef<SerializedAggregates>();
			SetAggregation(outOfCoreAgts);
			outOfCoreAgts->GetAggregates().push_back(mrtAgt);
			outOfCoreAgts->GetAggregates().push_back(raycastAgt);

			Context()->Update();

		}

		void VM_LargeVolumeRayCast::UpdateScene()
		{

		}

		void VM_LargeVolumeRayCast::DestroyEvent()
		{
			rayCastShading.reset();
			oocPrimitive.reset();
			VisualMan::DestroyEvent();
		}

		void VM_LargeVolumeRayCast::MouseWheelEvent(int ydegree, int xdegree)
		{
			const auto fov = mrtAgt->CreateGetCamera()->GetFov();
			mrtAgt->CreateGetCamera()->SetFov(fov + ydegree);
			Context()->Update();
		}

		void VM_LargeVolumeRayCast::FileDropEvent(const std::vector<std::string>& fileNames)
		{
			for (const auto & each : fileNames)
			{
				auto extension = each.substr(each.find_last_of('.'));
				bool found = false;
				if (extension == ".lvd")
				{
					SetupResources(each);
					found = true;
				}
				else if (extension == ".tf")
				{
					SetupTF(each);
					found = true;
				}
				if (found)
					break;
			}
			Context()->Update();
		}

		void VM_LargeVolumeRayCast::ResizeEvent(int w, int h)
		{
			VisualMan::ResizeEvent(w, h);
			if (raycastAgt)
			{
				raycastAgt->CreateGetCamera()->GetViewport()->SetWidth(w);		// only for clear use
				raycastAgt->CreateGetCamera()->GetViewport()->SetHeight(h);
			}
			if(mrtAgt)
			{
				mrtAgt->CreateGetCamera()->GetViewport()->SetWidth(w);
				mrtAgt->CreateGetCamera()->GetViewport()->SetHeight(h);
			}
			Context()->Update();
		}

		void VM_LargeVolumeRayCast::MouseMoveEvent(MouseButton button, int xpos, int ypos)
		{
			VisualMan::MouseMoveEvent(button, xpos, ypos);
			Context()->Update();
		}

		void VM_LargeVolumeRayCast::UpdateEvent()
		{
			VisualMan::UpdateEvent();
			std::string title = "LVD Render -- fps:"+ std::to_string(GetFPS());
			Context()->SetWindowTitle(title);
		}

		void VM_LargeVolumeRayCast::SetupShading()
		{

		}

		void VM_LargeVolumeRayCast::SetupResources(const std::string& fileName)
		{
			assert(oocPrimitive);
			assert(rayCastShading);
			Ref<OutOfCoreVolumeTexture> oocResources;

			try
			{
				oocResources = MakeRef<OutOfCoreVolumeTexture>(fileName);
			}
			catch (std::runtime_error &e)
			{
				Warning("Can not load lvd file");
				return;
			}

			rayCastShading->CreateGetTextureSampler(1)->SetTexture(oocResources->GetVolumeTexture(0));
			rayCastShading->CreateGetTextureSampler(2)->SetTexture(oocResources->GetVolumeTexture(1));
			rayCastShading->CreateGetTextureSampler(3)->SetTexture(oocResources->GetVolumeTexture(2));
			rayCastShading->CreateGetTextureImageUnit(1)->SetTexture(oocResources->GetMappingTableTexture());
			rayCastShading->CreateGetAtomicCounter(3)->SetBufferObject(oocResources->GetAtomicCounterBuffer());
			rayCastShading->CreateGetSSBO(0)->SetBufferObject(oocResources->GetHashBuffer());
			rayCastShading->CreateGetSSBO(1)->SetBufferObject(oocResources->GetBlockIDBuffer());

			auto v = oocResources->VirtualBlockDim();
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("totalPageTableSize")->SetUniform3i(1, v.Data());
			v = oocResources->DataResolution();
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("volumeDataSizeNoRepeat")->SetUniform3i(1, v.Data());
			v = oocResources->BlockSize() - 2 * oocResources->Padding();
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("blockDataSizeNoRepeat")->SetUniform3i(1, v.Data());
			v = oocResources->Padding();
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("repeatOffset")->SetUniform3i(1, v.Data());
			oocPrimitive->SetOutOfCoreResources(oocResources);

			Context()->Update();
		}

		void VM_LargeVolumeRayCast::SetupTF(const std::string& fileName)
		{
			assert(rayCastShading);
			try
			{
				const auto tfTex = MakeTransferFunction1DTexture(fileName);
				rayCastShading->CreateGetTextureSampler(4)->SetTexture(tfTex);
			}
			catch (std::runtime_error & e)
			{
				Warning("Can not load .tf file");
				return;
			}

			Context()->Update();
		}
	}
}

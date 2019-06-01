
#include "vm_multipassraycast.h"
#include "shaderprogram.h"
#include "framebufferobject.h"
#include "rendercontext.h"
#include "blitframebuffer.h"
#include "actoreventcallback.h"
#include "trivialscenemanager.h"

namespace ysl {
	namespace vm
	{
		void VM_VolumeRayCast2::InitEvent()
		{
			// Setup the aggregation and the scene manager
			SetAggregation(MakeRef<Aggregate>());
			auto sceneManager = MakeRef<TrivialSceneManager>();
			GetAggregate()->SceneManager().push_back(sceneManager);
			//manipulator->SetCamera(GetAggregate()->GetCamera());
			BindCameraEvent(GetAggregate()->GetCamera());

			// Read a raw volume and tf function
			//Vec3i volSize{ 480,720,120 };
			//auto volumeTex = MakeVolumeTexture(R"(D:\scidata\combustion\mixfrac.raw)", volSize.x, volSize.y, volSize.z);
			//auto tfTex = MakeTransferFunction1D(R"(D:\scidata\elt_tf1d2.TF1D)");

			// Create and setup a shading for the first pass
			auto pass0Shading = MakeRef<Shading>();
			//pass0Shading->CreateGetEnableStateSet()->Enable(EN_DEPTH_TEST);
			//pass0Shading->CreateGetEnableStateSet()->Enable(EN_CULL_FACE);
			//pass0Shading->CreateGetRenderStateSet()->SetRenderState(MakeRef<DepthFuncState>(FU_ALWAYS), -1);
			//pass0Shading->CreateGetEnableStateSet()->Disable(EN_CULL_FACE);
			//pass0Shading->CreateGetRenderStateSet()->SetRenderState(MakeRef<CullFaceState>(PF_FRONT),-1);

			auto positionGLSL = pass0Shading->CreateGetProgram();
			auto fragShader = MakeRef<GLSLFragmentShader>();
			fragShader->SetFromFile(R"(D:\code\MRE\resource\glsl\position_f.glsl)");
			auto vertShader = MakeRef<GLSLVertexShader>();
			vertShader->SetFromFile(R"(D:\code\MRE\resource\glsl\position_v.glsl)");
			positionGLSL->AttachShader(fragShader);
			positionGLSL->AttachShader(vertShader);
			//positionGLSL->BindFragDataLocation(0, "entryPos");
			//positionGLSL->BindFragDataLocation(1, "exitPos");


			// Create and setup a shading for the second pass
			//auto pass1Shading = MakeRef<Shading>();
			//auto raycastGLSL = pass1Shading->CreateGetProgram();
			//auto fs = MakeRef<GLSLFragmentShader>();
			//fs->SetFromFile(R"(D:\code\MRE\resource\glsl\raycast_fs.glsl)");
			//auto vs = MakeRef<GLSLFragmentShader>();
			//vs->SetFromFile(R"(D:\code\MRE\resource\glsl\raycast_vs.glsl)");
			//pass1Shading->CreateGetTextureSampler(0)->SetTexture(volumeTex);
			//pass1Shading->CreateGetTextureSampler(1)->SetTexture(tfTex);
			// Create a artist
			auto artist = MakeRef<Artist>();
			artist->GetLOD(0)->push_back(pass0Shading);

			// Create an actor and add it into scene manager
			auto actor = MakeRef<Actor>(nullptr,artist, nullptr);

			/*actor->CreateGetUniformSet()->CreateGetUniform("step")->SetUniformValue(0.001f);
			actor->CreateGetUniformSet()->CreateGetUniform("ka")->SetUniformValue(1.0f);
			actor->CreateGetUniformSet()->CreateGetUniform("kd")->SetUniformValue(1.0f);
			actor->CreateGetUniformSet()->CreateGetUniform("ks")->SetUniformValue(50.f);
			actor->CreateGetUniformSet()->CreateGetUniform("texTransfunc")->SetUniformValue(1);
			actor->CreateGetUniformSet()->CreateGetUniform("texVolume")->SetUniformValue(0);*/

			auto raycast = MakeRef<RayCast2ActorEventCallback>();
			raycast->BindToActor(actor);
			sceneManager->AddActor(actor);

			// Create position Texture
			auto w = GetAggregate()->GetCamera()->GetViewport()->GetWidth();
			auto h = GetAggregate()->GetCamera()->GetViewport()->GetHeight();
			Vec2i vSize{ w,h };

			entryTexture = MakeRef<Texture>();
			auto texParam = MakeRef<TexCreateParams>();
			texParam->SetSize(vSize.x, vSize.y, 0);
			texParam->SetTextureFormat(TF_RGB32F);
			texParam->SetTextureTarget(TD_TEXTURE_2D);
			entryTexture->SetSetupParams(texParam);
			entryTexture->CreateTexture();	// needless
			exitTexture = MakeRef<Texture>();
			exitTexture->SetSetupParams(texParam);
			exitTexture->CreateTexture();

			// Create FBO
			//auto fbo = Context()->CreateFramebufferObject(vSize.x, vSize.y, RDB_COLOR_ATTACHMENT0, RDB_COLOR_ATTACHMENT0);
			//fbo->SetDrawBuffers(RDB_COLOR_ATTACHMENT0, RDB_COLOR_ATTACHMENT1);
			//auto attach0 = MakeRef<FBOTextureAttachment>(entryTexture);
			//auto attach1 = MakeRef<FBOTextureAttachment>(exitTexture);
			//auto sdAttach = MakeRef<FBODepthStencilAttachment>();
			//fbo->AddTextureAttachment(AP_COLOR_ATTACHMENT0, attach0);
			//fbo->AddTextureAttachment(AP_COLOR_ATTACHMENT1, attach1);
			//fbo->AddDepthStencilAttachment(sdAttach);

			//auto blit = MakeRef<BlitFramebufferEvent>(fbo, Bound2i{ {0,0},{800,600} }, Context()->GetFramebuffer(), Bound2i{ {0,0},{800,600} }, VM_BB_COLOR_BUFFER_BIT);
			// add the blit to the renderer

			//GetAggregate()->Renderers()[0]->SetFramebuffer(fbo);
			GetAggregate()->Renderers()[0]->SetFramebuffer(Context()->GetFramebuffer());
			//GetAggregate()->Renderers()[0]->AddRenderFinishedEventCallback(blit);
			GetAggregate()->GetCamera()->GetViewport()->SetClearFlag(CF_CLEAR_COLOR_DEPTH);
			GetAggregate()->GetCamera()->GetViewport()->SetClearColor(Vec4f{1.0,1.0,1.0,1.0});

		}

		void VM_VolumeRayCast2::SetupAggregate()
		{
		
		}
	}
}


#include "vm_multipassraycast.h"
#include "shaderprogram.h"
#include "framebufferobject.h"
#include "rendercontext.h"
#include "blitframebuffer.h"
#include "actoreventcallback.h"
#include "trivialscenemanager.h"
#include "screenactor.h"

namespace ysl {
	namespace vm
	{
		void VM_VolumeRayCast2::InitEvent()
		{
			auto mrtAggregate = MakeRef<Aggregate>();
			mrtAggregate->SceneManager().push_back(MakeRef<TrivialSceneManager>());
			BindCameraEvent(mrtAggregate->CreateGetCamera());
			/// Step 1.
			// Create and setup 
			auto positionShading = MakeRef<Shading>();
			//positionShading->CreateGetEnableStateSet()->Disable(EN_DEPTH_TEST);
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
			positionGLSL->BindFragDataLocation(0, "entryPos");
			positionGLSL->BindFragDataLocation(1, "exitPos");

			// Create a artist
			auto artist = MakeRef<Artist>();
			artist->GetLOD(0)->push_back(positionShading);
			auto scale = MakeRef<Transform>();
			scale->SetScale(Vec3f(160,240,40).Normalized());
			auto geometryActor = MakeRef<Actor>(nullptr, artist, scale);
			auto geometryActorCallback = MakeRef<RayCast2ActorEventCallback>();
			geometryActorCallback->BindToActor(geometryActor);
			std::static_pointer_cast<TrivialSceneManager>(mrtAggregate->SceneManager().at(0))->AddActor(geometryActor);

			// Create position Texture
			auto w = mrtAggregate->CreateGetCamera()->GetViewport()->GetWidth();
			auto h = mrtAggregate->CreateGetCamera()->GetViewport()->GetHeight();
			Vec2i vSize{ w,h };
			entryTexture = MakeRef<Texture>();
			auto texParam = MakeRef<TexCreateParams>();
			texParam->SetSize(vSize.x, vSize.y, 0);
			texParam->SetTextureFormat(TF_RGBA32F);
			texParam->SetTextureTarget(TD_TEXTURE_2D);
			entryTexture->SetSetupParams(texParam);
			entryTexture->CreateTexture();	// needless
			exitTexture = MakeRef<Texture>();
			exitTexture->SetSetupParams(texParam);
			exitTexture->CreateTexture();
			/// Step 2.
			// Setup a screen actor to run ray casting using another render aggregate
			auto mainAggregate = MakeRef<Aggregate>();
			mainAggregate->SceneManager().push_back(MakeRef<TrivialSceneManager>());
			// Read a raw volume and tf function
			Vec3i volSize{ 160,240,40 };
			auto volumeTex = MakeVolumeTexture(R"(data\mixfrac160x240x40.raw)", volSize.x, volSize.y, volSize.z);
			//auto tfTex = MakeTransferFunction1D(R"(D:\scidata\elt_tf1d2.TF1D)");
			auto tfTex = MakeTransferFunction1DTexture({Color::transparent,Color::blue,Color::yellow});

			auto rayCastShading = MakeRef<Shading>();
			auto raycastGLSL = rayCastShading->CreateGetProgram();
			auto fs = MakeRef<GLSLFragmentShader>();
			fs->SetFromFile(R"(glsl\raycast_fs.glsl)");
			auto vs = MakeRef<GLSLVertexShader>();
			vs->SetFromFile(R"(glsl\raycast_vs.glsl)");
			raycastGLSL->AttachShader(fs);
			raycastGLSL->AttachShader(vs);

			rayCastShading->CreateGetTextureSampler(1)->SetTexture(volumeTex);
			rayCastShading->CreateGetTextureSampler(2)->SetTexture(tfTex);
			rayCastShading->CreateGetTextureSampler(3)->SetTexture(entryTexture);
			rayCastShading->CreateGetTextureSampler(4)->SetTexture(exitTexture);

			rayCastShading->CreateGetUniformSet()->CreateGetUniform("step")->SetUniformValue(0.001f);
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("ka")->SetUniformValue(1.0f);
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("kd")->SetUniformValue(1.0f);
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("ks")->SetUniformValue(50.f);
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("texTransfunc")->SetUniformValue(2);
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("texVolume")->SetUniformValue(1);
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("texStartPos")->SetUniformValue(3);
			rayCastShading->CreateGetUniformSet()->CreateGetUniform("texEndPos")->SetUniformValue(4);

			//rayCastShading->CreateGetEnableStateSet()->Enable(EN_BLEND);

			auto effect2 = MakeRef<Artist>();
			effect2->GetLOD(0)->push_back(rayCastShading);

			auto screenActor = MakeRef<Actor>(nullptr, effect2, nullptr);
			auto screenActorCallback = MakeRef<ScreenActorEventCallback>();
			screenActorCallback->BindToActor(screenActor);
			std::static_pointer_cast<TrivialSceneManager>(mainAggregate->SceneManager().at(0))->AddActor(screenActor);
			/// Step 3. Pipline Configuraion

			//Create FBO
			auto fbo = Context()->CreateFramebufferObject(vSize.x, vSize.y, RDB_COLOR_ATTACHMENT0, RDB_COLOR_ATTACHMENT0);
			fbo->SetDrawBuffers(RDB_COLOR_ATTACHMENT0, RDB_COLOR_ATTACHMENT1);
			const auto attach0 = MakeRef<FBOTextureAttachment>(entryTexture);
			const auto attach1 = MakeRef<FBOTextureAttachment>(exitTexture);

			auto sdAttach = MakeRef<FBODepthStencilAttachment>();
			fbo->AddTextureAttachment(AP_COLOR_ATTACHMENT0, attach0);
			fbo->AddTextureAttachment(AP_COLOR_ATTACHMENT1, attach1);


			mrtAggregate->Renderers().at(0)->SetFramebuffer(fbo);
			mrtAggregate->CreateGetCamera()->GetViewport()->SetClearFlag(CF_CLEAR_COLOR_DEPTH);
			mrtAggregate->CreateGetCamera()->GetViewport()->SetClearColor(Vec4f{ 0,0,0,0 });
			
			mainAggregate->CreateGetCamera()->GetViewport()->SetClearFlag(CF_CLEAR_COLOR_DEPTH);
			mainAggregate->Renderers().at(0)->SetFramebuffer(Context()->GetFramebuffer());

			auto serializedAggregate = MakeRef<SerializedAggregates>();
			serializedAggregate->GetAggregates().push_back(mrtAggregate);
			serializedAggregate->GetAggregates().push_back(mainAggregate);
			SetAggregation(serializedAggregate);
		}

		void VM_VolumeRayCast2::SetupAggregate()
		{

		}
	}
}

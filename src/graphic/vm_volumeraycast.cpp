
#include "vm_volumeraycast.h"
#include "rendercontext.h"
#include "shaderprogram.h"
#include "texture.h"
#include "trivialscenemanager.h"
#include "drawelements.h"

namespace ysl
{
	namespace vm
	{
		RayCastActorEventCallback::RayCastActorEventCallback()
		{
			proxyGeometry = MakeRef<Primitive>();
			Bound3f bound({0,0,0}, {1,1,1});
			Point3f points[8];
			for (int i = 0; i < 8; i++)
			{
				points[i] = bound.Corner(i);
				std::cout << points[i];
			}
			unsigned int indices[] = {0,2,1,1,2,3,
				4,5,6,5,7,6,
				0,1,4,1,5,4,
				2,6,3,3,6,7,
				0,4,2,2,4,6,
				1,3,5,3,7,5};
			auto vertexIndex = MakeRef<ArrayUInt>();
			vertexIndex->GetBufferObject()->SetLocalData(indices, sizeof(indices));
			vertexArray = MakeRef<ArrayFloat3>();
			texCoordArray = MakeRef<ArrayFloat3>();
			vertexArray->GetBufferObject()->SetLocalData(points, sizeof(points));
			texCoordArray->GetBufferObject()->SetLocalData(points, sizeof(points));

			proxyGeometry->SetVertexPositionArray(vertexArray);
			proxyGeometry->SetVertexTexCoordArray(texCoordArray);

			auto drawCall = MakeRef<DrawElementsUInt>();

			drawCall->SetIndexBuffer(vertexIndex);
			proxyGeometry->DrawCalls().push_back(drawCall);
				
		}
		void RayCastActorEventCallback::OnActorRenderStartedEvent(Actor * actor,
			const Camera* camera,
			Renderable *,
			const Shading* shading,
			int pass)
		{
			assert(pass == 0);
			(void)pass;
			if (shading)
			{
				auto const program = shading->GetProgram();
				const auto eyePos = camera->Position();
				const auto eye_position = program->GetGenericUniformLocation("eye_position");
				if(eye_position != -1)
				actor->CreateGetUniformSet()->CreateGetUniform("eye_position")->SetUniform3f(1, eyePos.ConstData());;
				// update light dir and halfway 
			}
		}
		void RayCastActorEventCallback::BindToActor(Ref<Actor> actor)
		{
			if (actor)
			{
				auto shared_this = std::static_pointer_cast<RayCastActorEventCallback>(shared_from_this());
				actor->RemoveActorRenderEventCallback(shared_this);
				actor->AddActorRenderEventCallback(shared_this);
				actor->SetRenderable(proxyGeometry, 0);
			}
		}

		void VM_VolumeRayCast::UpdateScene()
		{

		}

		void VM_VolumeRayCast::InitEvent()
		{
			//
			auto shading = MakeRef<Shading>();
			auto glsl = shading->CreateGetProgram();
			auto fragShader = MakeRef<GLSLFragmentShader>();
			fragShader->SetFromFile(R"(D:\code\MRE\resource\glsl\singlepassraycast_fs.glsl)");
			auto vertShader = MakeRef<GLSLVertexShader>();
			vertShader->SetFromFile(R"(D:\code\MRE\resource\glsl\singlepassraycast_vs.glsl)");
			glsl->AttachShader(fragShader);
			glsl->AttachShader(vertShader);
			// Read volume data
			Size3 volSize{ 480,720,120 };
			auto volumeTex = MakeVolumeTexture(R"(D:\scidata\combustion\mixfrac.raw)", volSize.x,volSize.y,volSize.z);
			// Read transfer function
			auto tfTex = MakeTransferFunction1D(R"(D:\tf1.tfi)");

			auto effect = MakeRef<Artist>();
			effect->GetLOD(0)->push_back(shading);
			auto actor = MakeRef<Actor>(nullptr,effect,nullptr);
			// Set illumination parameters
			actor->CreateGetUniformSet()->CreateGetUniform("step")->SetUniformValue(0.001f);
			actor->CreateGetUniformSet()->CreateGetUniform("ka")->SetUniformValue(1.0f);
			actor->CreateGetUniformSet()->CreateGetUniform("kd")->SetUniformValue(1.0f);
			actor->CreateGetUniformSet()->CreateGetUniform("ks")->SetUniformValue(50.0f);
			actor->CreateGetUniformSet()->CreateGetUniform("texTransfunc")->SetUniformValue(1);
			actor->CreateGetUniformSet()->CreateGetUniform("texVolume")->SetUniformValue(0);
			// Set texture samplers
			shading->CreateGetTextureSampler(0)->SetTexture(volumeTex);
			shading->CreateGetTextureSampler(1)->SetTexture(tfTex);
			shading->CreateGetRenderStateSet()->SetRenderState(MakeRef<CullFaceState>(), -1);

			// Enable
			shading->CreateGetEnableStateSet()->Enable(EN_CULL_FACE);
			shading->CreateGetEnableStateSet()->Enable(EN_DEPTH_TEST);
			shading->CreateGetEnableStateSet()->Enable(EN_BLEND);
			shading->CreateGetRenderStateSet()->SetRenderState(MakeRef<BlendFuncState>(BF_SRC_COLOR,BF_SRC_ALPHA,BF_ONE_MINUS_SRC_COLOR,BF_ONE_MINUS_SRC_ALPHA), -1);

			auto rayCast = MakeRef<RayCastActorEventCallback>();
			rayCast->BindToActor(actor);
			// Create a frame and a scene manager
			//aggregate = MakeRef<Aggregate>();
			SetAggregation(MakeRef<Aggregate>());
			auto sceneManager = MakeRef<TrivialSceneManager>();
			sceneManager->AddActor(actor);
			GetAggregate()->SceneManager().push_back(sceneManager);
			// Set Clear flags
			GetAggregate()->GetCamera()->GetViewport()->SetClearFlag(CF_CLEAR_COLOR_DEPTH);
			// Set camera under control
			//manipulator->SetCamera(aggregate->GetCamera());
			BindCameraEvent(GetAggregate()->GetCamera());


			assert(Context());
			GetAggregate()->Renderers()[0]->SetFramebuffer(Context()->GetFramebuffer());
		}
	}
}

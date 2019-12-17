#include "vm_mesh.h"

#include <VMUtils/log.hpp>
#include "abstraarray.h"
#include "primitive.h"
#include "renderstudio.h"
#include "shaderprogram.h"
#include "trivialscenemanager.h"
#include "actor.h"
#include "rendercontext.h"

	namespace vm
	{

		void VM_Mesh::InitEvent()
		{
			//InitDefault();
			auto  vertShader = MakeVMRef<vm::GLSLVertexShader>();
			vertShader->SetFromFile(R"(glsl\trivial_vs.glsl)");
			assert(vertShader->Compile());
			auto fragShader = MakeVMRef<vm::GLSLFragmentShader>();
			fragShader->SetFromFile(R"(glsl\trivial_fs.glsl)");
			assert(fragShader->Compile());
			//Vec3f viewPos{ 0,0,5 };
			//bunnyPrimitive = MakePrimitive(R"(D:\Desktop\bunny1.obj)");
			bunnyPrimitive = MakeCube(Bound3f{ {0,0,0},{1,1,1} });

			//auto teapotPrimitive = MakePrimitive(R"(D:\Desktop\teapot.obj)");


			auto abstraAggregate = MakeVMRef<vm::Aggregate>();

			auto triSceneMnger = MakeVMRef<vm::TrivialSceneManager>();

			abstraAggregate->SceneManager().push_back(triSceneMnger);

			auto artist = MakeVMRef<vm::Artist>();

			auto shading = MakeVMRef<vm::Shading>();

			shading->CreateGetProgram()->AttachShader(vertShader);

			shading->CreateGetProgram()->AttachShader(fragShader);

			Vec3f lightColor{ 1.0,1.0,1.0 };
			Vec3f lightPos{ 10,10,5.0 };
			Vec4f objectColor{ .4,0.3,0.8,1};

			auto scale = MakeVMRef<Transform>();

			//scale->SetScale(Vec3f{ 480,720,120 }.Normalized());

			const auto actor1 = MakeVMRef<vm::Actor>(bunnyPrimitive, artist, scale);

			
			//const auto actor2 = MakeRef<vm::Actor>(teapotPrimitive, artist, nullptr);


			shading->CreateGetUniformSet()->CreateGetUniform("light_pos")->SetUniform3f(1, lightPos.Data());
			shading->CreateGetUniformSet()->CreateGetUniform("light_color")->SetUniform3f(1, lightColor.Data());
			shading->CreateGetUniformSet()->CreateGetUniform("object_color")->SetUniform4f(1, objectColor.Data());

			//actor2->CreateGetUniformSet()->CreateGetUniform("light_pos")->SetUniform3f(1, lightPos.Data());
			//actor2->CreateGetUniformSet()->CreateGetUniform("light_color")->SetUniform3f(1, lightColor.Data());
			//actor2->CreateGetUniformSet()->CreateGetUniform("object_color")->SetUniform4f(1, objectColor.Data());


			shading->CreateGetEnableStateSet()->Enable(EN_DEPTH_TEST);
			shading->CreateGetEnableStateSet()->Enable(EN_CULL_FACE);
			shading->CreateGetRenderStateSet()->SetRenderState(MakeVMRef<CullFaceState>(PF_FRONT), -1);
			auto actorEvent = MakeVMRef<MarchingCubeActorCallback>();
			actor1->AddActorRenderEventCallback(actorEvent);

			//actor2->AddActorRenderEventCallback(actorEvent);

			abstraAggregate->CreateGetCamera()->GetViewport()->SetClearFlag(CF_CLEAR_COLOR_DEPTH);
			abstraAggregate->CreateGetCamera()->GetViewport()->SetClearColor(Vec4f{ 1,1,1,1 });

			manipulator->SetCamera(abstraAggregate->CreateGetCamera());

			artist->GetLOD(0)->push_back(shading);		//single pass

			triSceneMnger->AddActor(actor1);
			//triSceneMnger->AddActor(actor2);



			assert(Context());
			abstraAggregate->Renderers()[0]->SetFramebuffer(Context()->GetFramebuffer());

			this->abstraAggregate = abstraAggregate;
		}

		void VM_Mesh::UpdateScene()
		{

		}

	}

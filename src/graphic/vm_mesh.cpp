#include "vm_mesh.h"

#include "../utility/error.h"
#include "abstraarray.h"
#include "drawarray.h"
#include "primitive.h"
#include "renderstudio.h"
#include "shaderprogram.h"
#include "trivialscenemanager.h"
#include "actor.h"
#include "texture.h"
#include "drawelements.h"

namespace ysl
{
	namespace vm
	{
		void MarchingCubeActorCallback::OnActorRenderStartedEvent(Actor* actor, const Camera* camera,
			Renderable* renderable, const Shading* shading, int pass)
		{
			if(shading)
			{
				auto program = shading->GetProgram();

				const int loc = program->GetGenericUniformLocation("view_pos");
				if(loc != -1)
				{
					auto view_pos = camera->Position();
					actor->CreateGetUniformSet()->CreateGetUniform("view_pos")->SetUniform3f(1,view_pos.Data());
				}

			}
		}

		void VM_Mesh::InitEvent()
		{
			//InitDefault();

			auto  vertShader = MakeRef<vm::GLSLVertexShader>();
			vertShader->SetFromFile(R"(D:\code\MRE\resource\glsl\trivial_vs.glsl)");
			assert(vertShader->Compile());
			auto fragShader = MakeRef<vm::GLSLFragmentShader>();
			fragShader->SetFromFile(R"(D:\code\MRE\resource\glsl\trivial_fs.glsl)");
			assert(fragShader->Compile());

			//float vertices[] = 
			//{
			//	0.5f,  0.5f, 0.0f,  // top right
			//	 0.5f, -0.5f, 0.0f,  // bottom right
			//	-0.5f, -0.5f, 0.0f,  // bottom left
			//	-0.5f,  0.5f, 0.0f   // top left 
			//};
			//unsigned int indices[] = 
			//{  // note that we start from 0!
			//	0, 1, 3,  // first Triangle
			//	1, 2, 3   // second Triangle
			//};

			//auto primitive = MakeRef<vm::Primitive>();
			//auto vert = MakeRef<vm::ArrayFloat3>();
			//vert->GetBufferObject()->SetBufferData(sizeof(vertices), vertices, BU_STATIC_DRAW);
			//primitive->SetVertexArray(vert);
			//auto indexArray = MakeRef<ArrayUInt>();
			//indexArray->GetBufferObject()->SetBufferData(sizeof(indices), indices, BU_STATIC_DRAW);
			//auto elemDraw = MakeRef<DrawElementsUInt>();
			//elemDraw->SetIndexBuffer(indexArray);
			//primitive->DrawCalls().push_back(elemDraw);

			auto primitive = MakePrimitive(R"(D:\Desktop\teapot.obj)");

			aggregate = MakeRef<vm::Aggregate>();

			auto triSceneMnger = MakeRef<vm::TrivialSceneManager>();

			aggregate->SceneManager().push_back(triSceneMnger);

			auto artist = MakeRef<vm::Artist>();

			auto shading = MakeRef<vm::Shading>();

			shading->CreateGetProgram()->AttachShader(vertShader);

			shading->CreateGetProgram()->AttachShader(fragShader);

			Vec3f lightColor{ 1.0,1.0,1.0 };

			Vec3f lightPos{ 10,10,5.0 };

			//Vec3f viewPos{ 0,0,5 };

			Vec4f objectColor{ .4,0.3,0.8,1};

			const auto actor = MakeRef<vm::Actor>(primitive, artist, nullptr);

			actor->CreateGetUniformSet()->CreateGetUniform("light_pos")->SetUniform3f(1, lightPos.Data());

			actor->CreateGetUniformSet()->CreateGetUniform("light_color")->SetUniform3f(1, lightColor.Data());

			actor->CreateGetUniformSet()->CreateGetUniform("object_color")->SetUniform4f(1, objectColor.Data());

			shading->CreateGetEnableStateSet()->Enable(EN_DEPTH_TEST);

			auto actorEvent = MakeRef<MarchingCubeActorCallback>();

			actor->AddActorRenderEventCallback(actorEvent);

			aggregate->GetCamera()->GetViewport()->SetClearFlag(CF_CLEAR_COLOR_DEPTH);


			manipulator->SetCamera(aggregate->GetCamera());

			artist->GetLOD(0)->push_back(shading);		//single pass


			triSceneMnger->AddActor(actor);

		}

		void VM_Mesh::UpdateScene()
		{

		}


	}
}

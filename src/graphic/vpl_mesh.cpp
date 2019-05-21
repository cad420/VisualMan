#include "vpl_mesh.h"

#include "../utility/error.h"
#include "abstraarray.h"
#include "drawarray.h"
#include "primitive.h"
#include "renderstudio.h"
#include "shaderprogram.h"
#include "trivialscenemanager.h"
#include "actor.h"
#include "texture.h"

namespace ysl
{
	namespace vpl
	{
		void VPL_Mesh::InitEvent()
		{
			//InitDefault();

			float vertices[] = 
			{
				0.5f,  0.5f, 0.0f,  // top right
				 0.5f, -0.5f, 0.0f,  // bottom right
				-0.5f, -0.5f, 0.0f,  // bottom left
				-0.5f,  0.5f, 0.0f   // top left 
			};
			unsigned int indices[] = 
			{  // note that we start from 0!
				0, 1, 3,  // first Triangle
				1, 2, 3   // second Triangle
			};


			auto  vertShader = MakeRef<vpl::GLSLVertexShader>();
			vertShader->SetFromFile(R"(D:\code\MRE\resource\glsl\trivial_vs.glsl)");
			assert(vertShader->Compile());
			auto fragShader = MakeRef<vpl::GLSLFragmentShader>();
			fragShader->SetFromFile(R"(D:\code\MRE\resource\glsl\trivial_fs.glsl)");
			assert(fragShader->Compile());


			auto primitive = MakeRef<vpl::Primitive>();
			auto vert = MakeRef<vpl::ArrayFloat3>();
			vert->GetBufferObject()->SetBufferData(sizeof(vertices), vertices, BU_STATIC_DRAW);
			primitive->SetVertexArray(vert);
			primitive->DrawCalls().push_back(MakeRef<vpl::DrawArray>(0, 6));


			frame = MakeRef<vpl::Frame>();
			auto triSceneMnger = MakeRef<vpl::TrivialSceneManager>();
			frame->SceneManager().push_back(triSceneMnger);
			auto artist = MakeRef<vpl::Artist>();
			auto shading = MakeRef<vpl::Shading>();
			shading->CreateGetProgram()->AttachShader(vertShader);
			shading->CreateGetProgram()->AttachShader(fragShader);
			//shading->SetUniform(MakeRef<graphics::Uniform>("aColor"));

			Vec4f color{ 1.0,1.0,0.2,1.0 };
			shading->CreateGetUniform("aColor")->SetUniform4f(1, color.Data());

			 //Create a test texture
			//auto testTex = MakeRef<vpl::Texture>();
			//auto setupParams = MakeRef<vpl::TexCreateParams>();
			//setupParams->SetSize(5, 5, 5);
			//setupParams->SetTextureFormat(TF_RGBA32F);
			//setupParams->SetTextureTarget(TD_TEXTURE_3D);
			//testTex->SetSetupParams(setupParams);
			//assert(testTex->CreateTexture());
			//float data[125 * 4];
			//testTex->SetSubTextureData(data, IF_RGBA, IT_FLOAT, 0, 0, 0, 5, 5, 5);
			//auto texUnit = 0;
			//shading->CreateGetUniform("testTex")->SetUniform1i(1, &texUnit);
			//shading->CreateGetTextureSampler(0)->SetTexture(testTex);

			manipulator->SetCamera(frame->GetCamera());


			artist->GetLOD(0)->push_back(shading);

			auto actor = MakeRef<vpl::Actor>(primitive, artist,nullptr);
			triSceneMnger->AddActor(actor);
		}

		void VPL_Mesh::UpdateScene()
		{
			//ysl::Debug("VPL_Mesh::UpdateScene()");
		}
	}
}
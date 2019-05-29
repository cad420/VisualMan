
#include "shading.h"
#include "shaderprogram.h"

namespace ysl
{
	namespace vm
	{
		Ref<TextureSampler> Shading::CreateGetTextureSampler(int unitIndex)
		{
			auto texSampler = std::static_pointer_cast<TextureSampler>(
				CreateGetRenderStateSet()->
				GetRenderState(RS_TextureSampler, unitIndex)
				);
			if(texSampler == nullptr)
			{
				texSampler = MakeRef<TextureSampler>();
				CreateGetRenderStateSet()->SetRenderState(texSampler, unitIndex);
			}
			return texSampler;
		}



		Ref<Shading> MakePhongShading()
		{
			auto  vertShader = MakeRef<vm::GLSLVertexShader>();
			vertShader->SetFromFile(R"(D:\code\MRE\resource\glsl\trivial_vs.glsl)");
			auto fragShader = MakeRef<vm::GLSLFragmentShader>();
			fragShader->SetFromFile(R"(D:\code\MRE\resource\glsl\trivial_fs.glsl)");
			auto shading = MakeRef<vm::Shading>();

			shading->CreateGetProgram()->AttachShader(vertShader);
			shading->CreateGetProgram()->AttachShader(fragShader);
			shading->CreateGetEnableStateSet()->Enable(EN_DEPTH_TEST);

			Vec3f lightColor{1.0,1.0,1.0};
			Vec3f lightPos{10,10,5.0 };
			Vec4f objectColor{.4,0.3,0.8,1};

			shading->CreateGetUniformSet()->CreateGetUniform("light_pos")->SetUniform3f(1, lightPos.Data());
			shading->CreateGetUniformSet()->CreateGetUniform("light_color")->SetUniform3f(1, lightColor.Data());
			shading->CreateGetUniformSet()->CreateGetUniform("object_color")->SetUniform4f(1, objectColor.Data());
			return shading;
		}
	}
}


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

		Ref<TextureImageUnit> Shading::CreateGetTextureImageUnit(int unitIndex)
		{
			auto texImageUnit = std::static_pointer_cast<TextureImageUnit>(
				CreateGetRenderStateSet()->GetRenderState(RS_TextureImageUnit, unitIndex)
				);
			if(texImageUnit == nullptr)
			{
				texImageUnit = MakeRef<TextureImageUnit>();
				CreateGetRenderStateSet()->SetRenderState(texImageUnit, unitIndex);
			}
			return texImageUnit;
		}

		Ref<AtomicCounter> Shading::CreateGetAtomicCounter(int binding)
		{
			auto atomicCounter = std::static_pointer_cast<AtomicCounter>(
				CreateGetRenderStateSet()->GetRenderState(RS_AtomicCounterBuffer, binding)
				);
			if(!atomicCounter)
			{
				atomicCounter = MakeRef<AtomicCounter>();
				CreateGetRenderStateSet()->SetRenderState(atomicCounter, binding);
			}
			return atomicCounter;
		}

		Ref<ShaderStorageBufferObject> Shading::CreateGetSSBO(int binding)
		{
			auto ssbo = std::static_pointer_cast<ShaderStorageBufferObject>(
				CreateGetRenderStateSet()->GetRenderState(RS_ShaderStorageBuffer, binding)
				);
			if (!ssbo)
			{
				ssbo = MakeRef<ShaderStorageBufferObject>();
				CreateGetRenderStateSet()->SetRenderState(ssbo, binding);
			}
			return ssbo;
		}

		Ref<UniformBufferObject> Shading::CreateGetUBO(int binding)
		{
			auto ubo = std::static_pointer_cast<UniformBufferObject>(
				CreateGetRenderStateSet()->GetRenderState(RS_UniformBuffer, binding)
				);
			if (!ubo)
			{
				ubo = MakeRef<UniformBufferObject>();
				CreateGetRenderStateSet()->SetRenderState(ubo, binding);
			}
			return ubo;
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
			Vec4f objectColor{1,1,1,1};

			shading->CreateGetUniformSet()->CreateGetUniform("light_pos")->SetUniform3f(1, lightPos.Data());
			shading->CreateGetUniformSet()->CreateGetUniform("light_color")->SetUniform3f(1, lightColor.Data());
			shading->CreateGetUniformSet()->CreateGetUniform("object_color")->SetUniform4f(1, objectColor.Data());
			return shading;
		}
	}
}

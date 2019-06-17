
#ifndef _SHADING_H_
#define _SHADING_H_
#include "abstrarenderstate.h"
#include "uniform.h"
#include "enableset.h"
#include "renderstate.h"

#include "renderstateset.h"
#include <cassert>

namespace ysl
{
	namespace vm
	{
		class VISUALMAN_EXPORT_IMPORT Shading
		{
		public:
			Shading() = default;
			virtual ~Shading() = default;
			// Shading Program getter and setter
			Ref<GLSLProgram> CreateGetProgram() { return CreateGetRenderStateSet()->CreateGetProgram(); }
			Ref<GLSLProgram> GetProgram() { assert(GetRenderStateSet()); return GetRenderStateSet()->GetProgram(); }
			Ref<const GLSLProgram> GetProgram()const { assert(GetRenderStateSet()); return GetRenderStateSet()->GetProgram(); }
			void SetProgram(Ref<GLSLProgram> program) { assert(GetRenderStateSet()); GetRenderStateSet()->SetProgram(program); }

			// RenderState getter and setter 
			Ref<RenderStateSet> CreateGetRenderStateSet() { return renderStateSet ? renderStateSet : (renderStateSet = MakeRef<RenderStateSet>()); }
			Ref<RenderStateSet> GetRenderStateSet()const { return renderStateSet; }
			void SetRenderState(Ref<RenderState> state) { CreateGetRenderStateSet()->SetRenderState(state, -1); }
			Ref<DepthFuncState> GetDepthFuncState(RenderStateType type) { return std::static_pointer_cast<DepthFuncState>(CreateGetRenderStateSet()->GetRenderState(type, -1)); }

			Ref<TextureSampler> CreateGetTextureSampler(int unitIndex);
			Ref<TextureImageUnit> CreateGetTextureImageUnit(int unitIndex);
			Ref<AtomicCounter> CreateGetAtomicCounter(int binding);
			Ref<ShaderStorageBufferObject> CreateGetSSBO(int binding);

			// EnableState getter and setter
			Ref<EnableStateSet> CreateGetEnableStateSet() { return enableSet ? enableSet : (enableSet = MakeRef<EnableStateSet>()); }
			Ref<EnableStateSet> GetEnableStateSet() { return enableSet; }
			Ref<const EnableStateSet> GetEnableStateSet()const { return enableSet; }

			void SetDisableState(EnableState enable) { CreateGetEnableStateSet()->Enable(enable); }
			void SetEnableState(EnableState enable) { CreateGetEnableStateSet()->Disable(enable); }

			Ref<UniformSet> CreateGetUniformSet() { return uniformSet ? uniformSet : (uniformSet = MakeRef<UniformSet>()); }
			Ref<UniformSet> GetUniformSet() { return uniformSet; }
			Ref<const UniformSet> GetUniformSet()const { return uniformSet; }

			// Uniform getter and setter
			void SetUniform(Ref<Uniform> uniform) { uniformSet->SetUniform(std::move(uniform)); }
			Ref<Uniform> GetUniform(const char * name) { return uniformSet->GetUniform(name); }
			Ref<Uniform> CreateGetUniform(const char * name) { return CreateGetUniformSet()->CreateGetUniform(name); }
			// texture unit
		private:
			Ref<RenderStateSet> renderStateSet; // shading program is in it
			Ref<UniformSet> uniformSet;
			Ref<EnableStateSet> enableSet;
		};


		Ref<Shading> VISUALMAN_EXPORT_IMPORT MakePhongShading();
	}
}

#endif

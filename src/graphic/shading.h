
#ifndef _SHADING_H_
#define _SHADING_H_
#include "abstrarenderstate.h"
#include "uniform.h"
#include "enableset.h"
#include "renderstate.h"


namespace ysl
{
	namespace graphics
	{
		class Shading
		{
		public:
			Shading() = default;
			virtual ~Shading() = default;

			// Shading Program getter and setter
			Ref<GLSLProgram> CreateGetProgram() { return renderStateSet->CreateGetProgram(); }
			void SetProgram(Ref<GLSLProgram> program) { renderStateSet->SetProgram(program); }

			// RenderState getter and setter 
			Ref<RenderStateSet> CreateGetRenderStateSet() { return renderStateSet ? renderStateSet : (renderStateSet = MakeRef<RenderStateSet>()); }
			void SetRenderState(Ref<RenderState> state) { CreateGetRenderStateSet()->SetRenderState(state); }
			Ref<DepthFuncState> GetDepthFuncState(RenderStateType type) { return std::static_pointer_cast<DepthFuncState>(CreateGetRenderStateSet()->GetRenderState(type)); }

			// EnableState getter and setter
			Ref<EnableStateSet> CreateGetEnableStateSet() { return enableSet ? enableSet : (enableSet = MakeRef<EnableStateSet>()); }
			void SetDisableState(EnableState enable) { CreateGetEnableStateSet()->SetEnable(enable); }
			void SetEnableState(EnableState enable) { CreateGetEnableStateSet()->SetDisable(enable); }

			// Uniform getter and setter
			void SetUniform(Ref<Uniform> uniform);
			Ref<Uniform> GetUniform(const char * name);
			Ref<Uniform> CreateGetUniform(const char * name);

			// texture unit




		private:
			Ref<RenderStateSet> renderStateSet; // shading program is in it
			Ref<UniformSet> uniformSet;
			Ref<EnableStateSet> enableSet;
		};
	}
}

#endif

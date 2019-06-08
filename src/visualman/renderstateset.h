
#ifndef _RENDERSTATESET_H_
#define _RENDERSTATESET_H_
#include "graphictype.h"
#include "abstrarenderstate.h"

namespace ysl
{
	namespace vm
	{
		class GLSLProgram;

		class VISUALMAN_EXPORT_IMPORT RenderStateSet
		{
			friend class Aggregate;
			friend class RenderContext;
		public:
			RenderStateSet() = default;
			void SetRenderState(Ref<RenderState> state, int index);
			Ref<RenderState> GetRenderState(RenderStateType type, int index);
			void RemoveRenderState(RenderStateType type, int index);
			void SetProgram(Ref<GLSLProgram> program);
			Ref<GLSLProgram> CreateGetProgram();
			Ref<GLSLProgram> GetProgram();
			//const auto & RenderStateBox()const { return renderStates; }
			//std::vector<RenderStateBox> RenderStateBox() { return renderStates; }
		private:
			std::vector<RenderStateBox> renderStates;
			Ref<GLSLProgram> program;
		};

	}
}
#endif
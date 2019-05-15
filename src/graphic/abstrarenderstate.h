
#ifndef _ABSTRARENDERSTATE_H_
#define _ABSTRARENDERSTATE_H_
#include "graphictype.h"
#include "camera.h"
#include <vector>

namespace ysl
{
	namespace graphics
	{
		class RenderContext;
		class GLSLProgram;

		enum RenderStateType
		{
			RS_VertexAttrib,
			RS_VertexAttrib0 = RS_VertexAttrib,
			RS_VertexAttrib1,
			RS_VertexAttrib2,
			RS_VertexAttrib3,
			RS_VertexAttrib4,
			RS_VertexAttrib5,
			RS_VertexAttrib6,
			RS_VertexAttrib7,

			RS_AlphaFunc,
			RS_BlendEquation,
			RS_BlendFunc,
			RS_CullFace,
			RS_DepthFunc,
			RS_FrontFace,
			RS_PolygonMode,
			RS_LineWidth,
			RS_GLSLProgram,

			RS_TextureSampler,
			RS_TextureSampler0 = RS_TextureSampler + 0,
			RS_TextureSampler1 = RS_TextureSampler + 1,
			RS_TextureSampler2 = RS_TextureSampler + 2,
			RS_TextureSampler3 = RS_TextureSampler + 3,
			RS_TextureSampler4 = RS_TextureSampler + 4,
			RS_TextureSampler5 = RS_TextureSampler + 5,
			RS_TextureSampler6 = RS_TextureSampler + 6,
			RS_TextureSampler7 = RS_TextureSampler + 7,
			RS_TextureSampler8 = RS_TextureSampler + 8,
			RS_TextureSampler9 = RS_TextureSampler + 9,
			RS_TextureSampler10 = RS_TextureSampler + 10,
			RS_TextureSampler11 = RS_TextureSampler + 11,
			RS_TextureSampler12 = RS_TextureSampler + 12,
			RS_TextureSampler13 = RS_TextureSampler + 13,
			RS_TextureSampler14 = RS_TextureSampler + 14,
			RS_TextureSampler15 = RS_TextureSampler + 15,

			RS_NONE,
		};

		class GRAPHICS_EXPORT_IMPORT RenderState
		{
		public:
			RenderState() = delete;
			RenderState(RenderStateType type) :type(type) {}
			virtual void Apply(int index, Ref<Camera> camera, RenderContext * context)const = 0;
			RenderStateType Type()const { return type; }
			virtual ~RenderState() = default;
		private:
			RenderStateType type;
		};


		class GRAPHICS_EXPORT_IMPORT RenderStateNonIndexed :public RenderState
		{
		public:
			RenderStateNonIndexed(RenderStateType type) :RenderState(type) {}
		};

		class GRAPHICS_EXPORT_IMPORT RenderStateIndexed:public RenderState
		{
		public:
			RenderStateIndexed(RenderStateType type):RenderState(type){}
			int Index()const { return index; }
			void SetIndex(int index) { this->index = index; }
		private:
			int index = -1;
		};

		class GRAPHICS_EXPORT_IMPORT RenderStateSet
		{
		public:
			RenderStateSet() = default;
			void SetRenderState(Ref<RenderState> state);
			Ref<RenderState> GetRenderState(RenderStateType type);

			void SetProgram(Ref<GLSLProgram> program) { this->program = program; }
			Ref<GLSLProgram> CreateGetProgram(){if(program) return program;return program = MakeRef<GLSLProgram>();}
		private:
			std::vector<Ref<RenderState>> renderStates;
			Ref<GLSLProgram> program;
		};

	}
}

#endif
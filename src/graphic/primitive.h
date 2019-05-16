
#ifndef _MESH_H_
#define _MESH_H_
#include "renderable.h"
#include <vector>
#include <array>
#include "graphictype.h"
#include "abstrdraw.h"
#include "vertexattribsetinterface.h"

namespace ysl
{
	namespace graphics
	{

		class AbstraArray;
		class AbstraDrawCall;

		class GRAPHICS_EXPORT_IMPORT Primitive:public Renderable,
											   public IVertexAttribSet
		{
		public:
			Primitive() = default;
			virtual ~Primitive() = default;
			const std::vector<Ref<AbstrDrawCall>> & DrawCalls()const { return drawCalls; }
			std::vector<Ref<AbstrDrawCall>> & DrawCalls(){ return drawCalls; }
			void AddDrawCall(Ref<AbstraDrawCall> dc);
			// IVertexAttribSet
			void SetVertexArray(Ref<AbstraArray> data)override { vertexAttribArrays[VA_VertexPointAttrib] = std::move(data); }
			Ref<AbstraArray> GetVertexArray()override { return vertexAttribArrays[VA_VertexPointAttrib]; }
			void SetNormalArray(Ref<AbstraArray> data)override { vertexAttribArrays[VA_VertexNormalAttrib] = std::move(data); }
			Ref<AbstraArray> GetNormalArray()override { return vertexAttribArrays[VA_VertexNormalAttrib]; }
			void SetColorArray(Ref<AbstraArray> data)override { vertexAttribArrays[VA_VertexColorAttrib] = std::move(data); }
			Ref<AbstraArray> GetColorArray()override { return vertexAttribArrays[VA_VertexColorAttrib]; }
			void SetTexCoordArray(Ref<AbstraArray> data)override { vertexAttribArrays[VA_VertexTexCoordAttrib] = std::move(data); }
			Ref<AbstraArray> GetTexCoordArray()override { return vertexAttribArrays[VA_VertexTexCoordAttrib]; }
			void SetVertexAttribArray(int attribLocation, Ref<AbstraArray> data)override { vertexAttribArrays[attribLocation] = std::move(data); }
			Ref<AbstraArray> GetVertexAttribArray(int attribLocation)override { return vertexAttribArrays[attribLocation]; }

		protected:
			void Render_Implement(const Actor * actor, const RenderState* state, const Camera* camera, RenderContext * context)override;
		private:
			//static constexpr int MaxVertexAttribArray = 8;
			std::vector<Ref<AbstrDrawCall>> drawCalls;
			std::array<Ref<AbstraArray>, VA_VertexAttribArrayCount> vertexAttribArrays;
		};
	}
}

#endif
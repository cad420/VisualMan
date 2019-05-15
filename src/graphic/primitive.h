
#ifndef _MESH_H_
#define _MESH_H_
#include "renderable.h"
#include <vector>
#include "graphictype.h"
#include "abstrdraw.h"
#include "vertexattribsetinterface.h"
#include <array>

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

			// IVertexAttribSet
			void SetVertexArray(Ref<AbstraArray> data)override { vertexAttribArrays[0] = data; }
			Ref<AbstraArray> GetVertexArray()override { return vertexAttribArrays[0]; }
			void SetNormalArray(Ref<AbstraArray> data)override { vertexAttribArrays[1] = data; }
			Ref<AbstraArray> GetNormalArray()override { return vertexAttribArrays[1]; }
			void SetColorArray(Ref<AbstraArray> data)override { vertexAttribArrays[2] = data; }
			Ref<AbstraArray> GetColorArray()override { return vertexAttribArrays[2]; }
			void SetTexCoordArray(Ref<AbstraArray> data)override { vertexAttribArrays[3] = data; }
			Ref<AbstraArray> GetTexCoordArray()override { return vertexAttribArrays[3]; }
			void SetVertexAttribArray(int attribLocation, Ref<AbstraArray> data)override { vertexAttribArrays[attribLocation] = data; }
			Ref<AbstraArray> GetVertexAttribArray(int attribLocation)override { return vertexAttribArrays[attribLocation]; }

		protected:
			void Render_Implement(const Actor * actor, const RenderState* state, const Camera* camera, RenderContext * context)override;
		private:

			static constexpr int MaxVertexAttribArray = 8;
			std::vector<Ref<AbstrDrawCall>> drawCalls;
			std::array<Ref<AbstraArray>, MaxVertexAttribArray> vertexAttribArrays;
		};
	}
}

#endif
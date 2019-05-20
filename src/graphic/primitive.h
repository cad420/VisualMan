
#ifndef _PRIMITIVE_H_
#define _PRIMITIVE_H_
#include <vector>
#include <array>
#include "graphictype.h"
#include "abstrdraw.h"
#include "vertexattribsetinterface.h"
#include "renderable.h"

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
			virtual ~Primitive();
			const std::vector<Ref<AbstrDrawCall>> & DrawCalls()const { return drawCalls; }
			std::vector<Ref<AbstrDrawCall>> & DrawCalls(){ return drawCalls; }
			void AddDrawCall(Ref<AbstraDrawCall> dc);
			// IVertexAttribSet
			void SetVertexArray(Ref<AbstraArray> data)override { vertexAttribArrays[VA_VertexPositionAttrib] = std::move(data); bind2VAO(VA_VertexPositionAttrib); }
			Ref<AbstraArray> GetVertexArray()override { return vertexAttribArrays[VA_VertexPositionAttrib]; }
			Ref<const AbstraArray> GetVertexArray() const override { return vertexAttribArrays[VA_VertexPositionAttrib]; }


			void SetNormalArray(Ref<AbstraArray> data)override { vertexAttribArrays[VA_VertexNormalAttrib] = std::move(data); bind2VAO(VA_VertexNormalAttrib); }
			Ref<AbstraArray> GetNormalArray()override { return vertexAttribArrays[VA_VertexNormalAttrib]; }
			Ref<const AbstraArray> GetNormalArray() const override { return vertexAttribArrays[VA_VertexNormalAttrib]; }

			void SetColorArray(Ref<AbstraArray> data)override { vertexAttribArrays[VA_VertexColorAttrib] = std::move(data); bind2VAO(VA_VertexColorAttrib); }
			Ref<AbstraArray> GetColorArray()override { return vertexAttribArrays[VA_VertexColorAttrib]; }
			Ref<const AbstraArray> GetColorArray() const override { return vertexAttribArrays[VA_VertexColorAttrib]; }

			void SetTexCoordArray(Ref<AbstraArray> data)override { vertexAttribArrays[VA_VertexTexCoordAttrib] = std::move(data); bind2VAO(VA_VertexTexCoordAttrib); }
			Ref<AbstraArray> GetTexCoordArray()override { return vertexAttribArrays[VA_VertexTexCoordAttrib]; }
			Ref<const AbstraArray> GetTexCoordArray() const override { return vertexAttribArrays[VA_VertexTexCoordAttrib]; }


			void SetVertexAttribArray(int attribLocation, Ref<AbstraArray> data)override { vertexAttribArrays[attribLocation] = std::move(data); bind2VAO(attribLocation); }
			Ref<AbstraArray> GetVertexAttribArray(int attribLocation)override { return vertexAttribArrays[attribLocation]; }
			Ref<const AbstraArray> GetVertexAttribArray(int attribLocation) const override { return vertexAttribArrays[attribLocation]; }

		protected:
			void Render_Implement(const Actor * actor, const Shading * shading, const Camera* camera, RenderContext * context)const override;

			void bind2VAO(int attribLocation);
		private:
			//static constexpr int MaxVertexAttribArray = 8;

			unsigned int vaoHandle = 0;

			std::vector<Ref<AbstrDrawCall>> drawCalls;
			std::array<Ref<AbstraArray>, VA_VertexAttribArray_Count> vertexAttribArrays;
			std::array<bool, VA_VertexAttribArray_Count> boundToVAO = { false ,false,false,false};
		};
	}
}

#endif
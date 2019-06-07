
#ifndef _PRIMITIVE_H_
#define _PRIMITIVE_H_
#include <vector>
#include <array>
#include "graphictype.h"
#include "abstrdraw.h"
#include "vertexattribsetinterface.h"
#include "renderable.h"
#include <geometry.h>

namespace ysl
{
	namespace vm
	{

		class AbstraArray;
	//	class AbstraDrawCall;
		class IRenderEvent;

		class VISUALMAN_EXPORT_IMPORT Primitive:public Renderable,
											   public IVertexAttribSet
		{
		public:
			Primitive() = default;
			virtual ~Primitive();
			const std::vector<Ref<AbstrDrawCall>> & DrawCalls()const { return drawCalls; }
			std::vector<Ref<AbstrDrawCall>> & DrawCalls(){ return drawCalls; }
			//void AddDrawCall(Ref<AbstraDrawCall> dc);
			// IVertexAttribSet
			void SetVertexPositionArray(Ref<AbstraArray> data) override;

			Ref<AbstraArray> GetVertexArray()override { return vertexAttribArrays[VA_VertexPositionAttrib]; }
			Ref<const AbstraArray> GetVertexArray() const override { return vertexAttribArrays[VA_VertexPositionAttrib]; }


			void SetVertexNormalArray(Ref<AbstraArray> data) override;

			Ref<AbstraArray> GetNormalArray()override { return vertexAttribArrays[VA_VertexNormalAttrib]; }
			Ref<const AbstraArray> GetNormalArray() const override { return vertexAttribArrays[VA_VertexNormalAttrib]; }

			void SetVertexColorArray(Ref<AbstraArray> data) override;

			Ref<AbstraArray> GetColorArray()override { return vertexAttribArrays[VA_VertexColorAttrib]; }
			Ref<const AbstraArray> GetColorArray() const override { return vertexAttribArrays[VA_VertexColorAttrib]; }

			void SetVertexTexCoordArray(Ref<AbstraArray> data) override;

			Ref<AbstraArray> GetTexCoordArray()override { return vertexAttribArrays[VA_VertexTexCoordAttrib]; }
			Ref<const AbstraArray> GetTexCoordArray() const override { return vertexAttribArrays[VA_VertexTexCoordAttrib]; }
			void SetVertexAttribArray(VertexAttribArrayIndexType attribLocation, Ref<AbstraArray> data) override;

			Ref<AbstraArray> GetVertexAttribArray(int attribLocation)override { return vertexAttribArrays[attribLocation]; }
			Ref<const AbstraArray> GetVertexAttribArray(int attribLocation) const override { return vertexAttribArrays[attribLocation]; }

			void UpdateDirtyBufferObject(BufferObjectUpdateMode mode) override;

			

		protected:

			void Render_Implement(const Actor * actor, const Shading * shading, const Camera* camera, RenderContext * context)const override;

			void DestroyVAO();

			void bind2VAO(int attribLocation);

			/**
			 * \brief This function will bind vertex attribute arrays to a vao object. If there is no vao object, it will create one.
			 *	\a vaoCompletion will be set as true after call.
			 */
			void rebind2VAO();

			/**
			 * \brief  Indicates whether the vao is completed. Completion is broken when new vertex attribute array is set.
			 */
			bool IsVAOCompletion()const { return vaoCompletion; }

			unsigned int vaoHandle = 0;

			std::vector<Ref<AbstrDrawCall>> drawCalls;
			std::array<Ref<AbstraArray>, VA_VertexAttribArray_Count> vertexAttribArrays;

			std::array<bool, VA_VertexAttribArray_Count> boundToVAO = { false,false,false,false };
			bool vaoCompletion = false;

		private:

		};

		//class GRAPHICS_EXPORT_IMPORT OutOfCorePrimitive :public Primitive
		//{
		//public:
		//	void AddRenderCallback(Ref<IDrawCallEvent>  callback);
		//protected:
		//	void Render_Implement(const Actor* actor, const Shading* shading, const Camera* camera, RenderContext* context) const override;
		//	Ref<IDrawCallEvent> callback;
		//};


		Ref<Primitive> VISUALMAN_EXPORT_IMPORT MakePrimitive(const std::string & fileName);
		Ref<Primitive> VISUALMAN_EXPORT_IMPORT MakePrimitive(const float* position, size_t positionCount, const float* normals, size_t normalCount,
			const unsigned* index, size_t indexCount);

		Ref<Primitive> VISUALMAN_EXPORT_IMPORT MakeCube(const Bound3f& bound);

	}



}

#endif
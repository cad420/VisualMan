
#include "primitive.h"
#include "rendercontext.h"
#include "abstraarray.h"
#include "../opengl/openglutils.h"
#include "../utility/objreader.h"
#include "drawelements.h"
#include "abstrdraw.h"

namespace ysl
{
	namespace vm
	{
		Primitive::~Primitive()
		{
			if(vaoHandle)
			{
				GL(glDeleteVertexArrays(1, &vaoHandle));
				vaoHandle = 0;
			}
		}


		void Primitive::Render_Implement(const Actor * actor, const Shading * shading, const Camera* camera, RenderContext * context)const
		{
			//std::cout << "Primitive::Render_Implement";
			// Bind vbo interface to context
			// A primitive is an implementation of the VertexArray Interface 
			// call draw command

			assert(context);
			assert(vaoHandle != 0);
			context->Bind_VAO(vaoHandle);
			//context->BindVertexArray(this);

			for(const auto & dc:drawCalls)
			{
				dc->Render();
			}
		}

		void Primitive::bind2VAO(int attribLocation)
		{
			if(vaoHandle == 0)
			{
				GL(glGenVertexArrays(1,&vaoHandle));
				assert(vaoHandle);
			}
			GL(glBindVertexArray(vaoHandle));
			GL(glBindBuffer(GL_ARRAY_BUFFER, vertexAttribArrays[attribLocation]->GetBufferObject()->Handle()));
			GL(glVertexAttribPointer(attribLocation,
				vertexAttribArrays[attribLocation]->ComponentNum(),
				vertexAttribArrays[attribLocation]->Type(),
				GL_FALSE,
				0,(void*)0));
			GL(glEnableVertexAttribArray(attribLocation));
			GL(glBindVertexArray(0));
			//Debug("%d\n", vaoHandle);
		}

		Ref<Primitive> MakePrimitive(const std::string& fileName)
		{
			ObjReader objReader(fileName);

			if (objReader.IsLoaded() == false)
			{
				throw std::runtime_error("can not load .obj file");
			}

			auto vertex = MakeRef<ArrayFloat3>();
			vertex->GetBufferObject()->SetBufferData(objReader.GetVertexBytes(),
				objReader.GetVertices().data(), 
				BU_STATIC_DRAW);

			auto normals = MakeRef<ArrayFloat3>();
			normals->GetBufferObject()->SetBufferData(objReader.GetNormalBytes(), 
				objReader.GetNormals().data(),
				BU_STATIC_DRAW);

			auto vertexIndex = MakeRef<ArrayUInt>();
			vertexIndex->GetBufferObject()->SetBufferData(objReader.GetVertexIndicesBytes(), 
				objReader.GetFaceIndices().data(), 
				BU_STATIC_DRAW);


			auto primitive = MakeRef<Primitive>();
			primitive->SetVertexArray(vertex);
			primitive->SetNormalArray(normals);

			auto drawElemUi = MakeRef<DrawElementsUInt>(1);
			drawElemUi->SetIndexBuffer(vertexIndex);
			
			primitive->DrawCalls().push_back(drawElemUi);

			return primitive;
		}
	}
}

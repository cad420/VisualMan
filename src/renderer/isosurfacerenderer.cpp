
#include "isosurfacerenderer.h"
#include "../utility/objreader.h"
#include "../utility/error.h"

#include "../../lib/gl3w/GL/gl3w.h"
#include "openglutils.h"

namespace ysl
{
	namespace app
	{
		OITMeshRenderer::OITMeshRenderer(int argc, char** argv,int w,int h):ImGuiApplication(argc,argv,w,h),
			camera({0.0,0.0,5}),windowWidth(w),windowHeight(h)
		{

		}

		void OITMeshRenderer::InitializeOpenGLResources()
		{
			InitShader();
			CreateMesh(R"(C:\Users\ysl\Desktop\dragon_1.obj)");
			UpdateMatrix(windowWidth, windowHeight);

			CreateScreenQuads();
			GL_ERROR_REPORT;
			CreateFragmentBufferList(windowWidth,windowWidth);
			GL_ERROR_REPORT;
			CreateHeadPointerImageInitializer(windowWidth, windowHeight);
			GL_ERROR_REPORT;
			CreateAtomicCounter();
			GL_ERROR_REPORT;

			//UpdateScreenQuads(width,height);
		}


		void OITMeshRenderer::RenderLoop()
		{
			// Set Camera Uniform
			glEnable(GL_BLEND);

			// Clear HeadPointerImage

			ClearHeadPointerImage();
			ClearAtomicCounter();

			testShader.bind();

			float c[4] = { 0.3,0.3,0.3,0.3 };

			const auto view = camera.view().Matrix();

			testShader.setUniformValue("model_matrix",model.Matrix());
			testShader.setUniformValue("view_matrix", view);
			testShader.setUniformValue("projection_matrix", proj.Matrix());
			testShader.setUniformValue("light_pos", Vector3f{0.f,0.f,10.f});
			testShader.setUniformValue("light_color", Vector3f{1.0,1.0,1.0});
			testShader.setUniformValue("object_color", RGBASpectrum{c});
			testShader.setUniformValue("view_pos", camera.position());


			{
				oitListShader.bind();
				// vertex shader
				oitListShader.setUniformValue("model_matrix", model.Matrix());
				oitListShader.setUniformValue("view_matrix", camera.view().Matrix());
				oitListShader.setUniformValue("projection_matrix", proj.Matrix());

				// fragment shader
				oitListShader.setUniformValue("light_pos", Vector3f{ 0.f,0.f,10.f });
				oitListShader.setUniformValue("light_color", Vector3f{ 1.0,1.0,1.0 });
				oitListShader.setUniformValue("object_color", RGBASpectrum{ 0.3 });
				oitListShader.setUniformValue("view_pos", camera.position());

				for (auto & m : meshes)
				{
					m.vao->bind();
					glDrawElements(GL_TRIANGLES, m.indexCount, GL_UNSIGNED_INT, 0);
				}

				oitRenderShader.bind();
			}


			{
				oitRenderShader.bind();
				oitRenderShader.setUniformValue("orthoMatrix", ortho.Matrix());
				oitRenderShader.setUniformValue("viewMatrix", view);
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}

			glDisable(GL_BLEND);

		}

		void OITMeshRenderer::CreateImageListTexture(int width, int height)
		{
			imageList = OpenGLTexture::CreateTexture2DRect(OpenGLTexture::R32UI,
				OpenGLTexture::FilterMode::Linear,
				OpenGLTexture::FilterMode::Linear,
				OpenGLTexture::WrapMode::ClampToEdge,
				OpenGLTexture::WrapMode::ClampToEdge,
				OpenGLTexture::RED, OpenGLTexture::UInt32, width, height, nullptr);
			GL_ERROR_REPORT;
			imageList->BindToDataImage(1, 0, false, 0, OpenGLTexture::ReadAndWrite, OpenGLTexture::R32UI);


		}

		void OITMeshRenderer::CreateScreenQuads()
		{
			screenQuads.vao.create();
			screenQuads.vao.bind();

			screenQuads.vbo = std::make_shared<OpenGLBuffer>(OpenGLBuffer::BufferTarget::VertexArrayBuffer);

			screenQuads.vbo->Bind();

			float screenSize[] = 
			{
				0,0,
				windowWidth,0,
				0,windowHeight,
				0,windowHeight,
				windowWidth,0,
				windowWidth,windowHeight
			};

			screenQuads.vbo->AllocateFor(screenSize, sizeof(screenSize));
			screenQuads.vbo->VertexAttribPointer(0,
				2, 
				OpenGLBuffer::Float, 
				false,
				sizeof(float) * 2,
				reinterpret_cast<void*>(0));
			screenQuads.vbo->Unbind();
			screenQuads.vao.unbind();
		}

		void OITMeshRenderer::CreateFragmentBufferList(int width, int height)
		{
			//fragmentBufferListBuffer = std::make_shared<OpenGLBuffer>(OpenGLBuffer::BufferTarget::TextureBuffer, OpenGLBuffer::DynamicCopy);
			const auto size = std::size_t(width)*height * 3 * 4 * sizeof(unsigned int);
			fragmentBufferListBuffer = std::make_shared<OpenGLBuffer>(OpenGLBuffer::ShaderStorageBuffer, OpenGLBuffer::BufferUsage::DynamicCopy);
			GL_ERROR_REPORT;
			fragmentBufferListBuffer->AllocateFor(nullptr, size * 3);
			GL_ERROR_REPORT;
			fragmentBufferListBuffer->BindBufferBase(3);
			GL_ERROR_REPORT;

			fragmentBufferListTexture = std::make_shared<OpenGLTexture>(OpenGLTexture::TextureTarget::TextureBuffer);
			//fragmentBufferList = OpenGLTexture::CreateTextureBuffer(OpenGLTexture::R32UI, OpenGLTexture::Linear, OpenGLTexture::Linear, OpenGLTexture::ClampToEdge, OpenGLTexture::RED, OpenGLTexture::UInt32, size, nullptr);
			
			//fragmentBufferList->BindToDataImage(2, 0, false, 0, OpenGLTexture::ReadAndWrite, OpenGLTexture::R32UI);

			
			GL_ERROR_REPORT;
		}

		void OITMeshRenderer::CreateHeadPointerImageInitializer(int width, int height)
		{
			initializer = std::make_shared<OpenGLBuffer>(OpenGLBuffer::BufferTarget::PixelUnpackBuffer);
			ResizeInitializer(width, height);
		}

		void OITMeshRenderer::CreateAtomicCounter()
		{
			atomicCounter = std::make_shared<OpenGLBuffer>(OpenGLBuffer::BufferTarget::AtomicCounterBuffer);

			atomicCounter->BindBufferBase(0);

			ClearAtomicCounter();
		}


		void OITMeshRenderer::InitShader()
		{
			testShader.create();
			testShader.addShaderFromFile(R"(D:\code\MRE\src\shader\utils\phoneshadingvert.glsl)",ysl::ShaderProgram::ShaderType::Vertex);
			testShader.addShaderFromFile(R"(D:\code\MRE\src\shader\utils\phoneshadingfrag.glsl)",ysl::ShaderProgram::ShaderType::Fragment);
			testShader.link();

			oitListShader.create();
			oitListShader.addShaderFromFile(R"(D:\code\MRE\src\shader\utils\phoneshadingvert.glsl)",ysl::ShaderProgram::ShaderType::Vertex);
			oitListShader.addShaderFromFile(R"(D:\code\MRE\src\shader\utils\oitphase1_f.glsl)",ysl::ShaderProgram::ShaderType::Fragment);
			oitListShader.link();

			oitRenderShader.create();
			oitRenderShader.addShaderFromFile(R"(D:\code\MRE\src\shader\utils\screenquads_v.glsl)",ysl::ShaderProgram::ShaderType::Vertex);
			oitRenderShader.addShaderFromFile(R"(D:\code\MRE\src\shader\utils\oitphase2_f.glsl)",ysl::ShaderProgram::ShaderType::Fragment);
			oitRenderShader.link();

		}

		void OITMeshRenderer::SetShaderUniform()
		{
			testShader.bind();
			testShader.setUniformValue("light_pos", Vector3f{ 0.f,0.f,10.f });
			testShader.setUniformValue("light_color", Vector3f{ 1.0,1.0,1.0 });
			testShader.setUniformValue("object_color", RGBSpectrum{ 0.3 });
			testShader.setUniformValue("view_pos", camera.position());
		}

		void OITMeshRenderer::CreateMesh(const std::string& fileName)
		{
			ObjReader reader;

			reader.Load(fileName);

			if (!reader.IsLoaded())
				ysl::Warning("%s cannot be loaded\n", fileName.c_str());

			Mesh aMesh;

			aMesh.vao = std::make_shared<OpenGLVertexArrayObject>();
			aMesh.vao->create();
			aMesh.vao->bind();

			aMesh.vbo = std::make_shared<OpenGLBuffer>(OpenGLBuffer::BufferTarget::VertexArrayBuffer);

			const auto vertexSize = reader.GetVertexBytes();
			const auto normalSize = reader.GetNormalBytes();
			const auto textureSize = reader.GetTexCoordBytes();


			aMesh.vbo->Bind();
			aMesh.vbo->AllocateFor(nullptr, vertexSize + normalSize + textureSize);
			aMesh.vbo->SetSubData(reader.GetVertices().data(),vertexSize, 0);
			aMesh.vbo->SetSubData(reader.GetNormals().data(), normalSize, vertexSize);
			aMesh.vbo->SetSubData(reader.GetTexCoord().data(), textureSize, vertexSize + normalSize);

			// Vertex Attribute
			aMesh.vbo->VertexAttribPointer(0, 3, OpenGLBuffer::Float, false, sizeof(Point3f), nullptr);
			// Normal Attribute
			aMesh.vbo->VertexAttribPointer(1, 3, OpenGLBuffer::Float, false, sizeof(Vector3f), reinterpret_cast<void*>(vertexSize));
			// Texture Attribute
			aMesh.vbo->VertexAttribPointer(2, 2, OpenGLBuffer::Float, false, sizeof(Point2f), reinterpret_cast<void*>(vertexSize+normalSize));


			aMesh.ebo = std::make_shared<OpenGLBuffer>(OpenGLBuffer::BufferTarget::ElementArrayBuffer);
			aMesh.ebo->Bind();
			aMesh.indexCount = reader.GetFaceIndices().size();
			aMesh.ebo->AllocateFor(reader.GetFaceIndices().data(), reader.GetVertexIndicesBytes());
			meshes.push_back(aMesh);

		}

		void OITMeshRenderer::MousePressEvent(MouseEvent* event)
		{
			lastMousePos = event->pos();
		}

		void OITMeshRenderer::MouseMoveEvent(MouseEvent* event)
		{
			const auto & p = event->pos();
			// Update Camera
			const float dx = p.x - lastMousePos.x;
			const float dy = lastMousePos.y - p.y;
			if (dx == 0 && dy == 0)
				return;
			if ((event->buttons() & MouseEvent::LeftButton) && (event->buttons() & MouseEvent::RightButton))
			{
				const auto directionEx = camera.up()*dy + dx * camera.right();
				camera.movement(directionEx, 0.002);
			}

			else if (event->buttons() & MouseEvent::LeftButton)
			{
				camera.rotation(dx, dy);
			}
			else if (event->buttons() == MouseEvent::RightButton)
			{
				const auto directionEx = camera.front()*dy;
				camera.movement(directionEx, 0.01);
			}
			lastMousePos = p;
		}

		void OITMeshRenderer::WindowResizeEvent(ResizeEvent* event)
		{
			windowWidth = event->size().x;
			windowHeight = event->size().y;
			//
			const auto aspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
			UpdateMatrix(windowWidth,windowHeight);
			ResizeScreenQuads(windowWidth, windowHeight);
			ResizeInitializer(windowWidth, windowHeight);
			ResizeHeadPointerImage(windowWidth,windowHeight);
			ResizeFragmentBufferList(windowWidth, windowHeight);
		}


		void OITMeshRenderer::ResizeHeadPointerImage(int width, int height)
		{
			assert(imageList);
			imageList->SetData(OpenGLTexture::R32UI,
				OpenGLTexture::RED, 
				OpenGLTexture::UInt32, 
				width,
				height, 
				0, 
				nullptr);
		}

		void OITMeshRenderer::UpdateMatrix(int width, int height)
		{
			proj.SetGLPerspective(45.0f, float(width) / height, 0.01, 100);
			ortho.SetGLOrtho(0, width, 0, height, 0.01, 100);
		}

		void OITMeshRenderer::ResizeScreenQuads(int width, int height)
		{
			assert(screenQuads.vbo);
			screenQuads.vbo->Bind();
			float screenSize[] =
			{
				0,0,
				width,0,
				0,height,
				0,height,
				width,0,
				width,height
			};
			screenQuads.vbo->SetSubData(screenSize, sizeof(screenSize),0);
			screenQuads.vbo->Unbind();
		}

		void OITMeshRenderer::ResizeFragmentBufferList(int width, int height)
		{
			assert(fragmentBufferListBuffer);
			//fragmentBufferList->Bind();
			const auto size = std::size_t(width)*height * 3 * 4 * sizeof(unsigned int);
			//fragmentBufferListBuffer->SetData(OpenGLTexture::R32UI, OpenGLTexture::RED, OpenGLTexture::UInt32, size, 0, 0, nullptr);
			//fragmentBufferList->AllocateFor(nullptr, std::size_t(width)*height * 3 * 4 * sizeof(unsigned int));
			//GL_ERROR_REPORT;
			fragmentBufferListBuffer->Unbind();
		}

        //
		// 
		//
		void OITMeshRenderer::ResizeInitializer(int width, int height)
		{
			assert(initializer);
			const auto size = std::size_t(width)*height * 3 * 4 * sizeof(unsigned int);
			initializer->Bind();
			initializer->AllocateFor(nullptr, size);
			auto ptr = initializer->Map(OpenGLBuffer::ReadWrite);
			memset(ptr, 0xFF, size);
			initializer->Unmap();
		}

		void OITMeshRenderer::ClearAtomicCounter()
		{
			assert(atomicCounter);
			auto zero = 0;
			atomicCounter->Bind();
			atomicCounter->AllocateFor(&zero, sizeof(int));
			atomicCounter->Unbind();
		}

		void OITMeshRenderer::ClearHeadPointerImage()
		{
			assert(initializer);
			assert(imageList);

			initializer->Bind();
			imageList->Bind();
			imageList->SetSubData(OpenGLTexture::RGBA, 
				OpenGLTexture::UInt32, 
				windowWidth, 0, windowHeight, 0,
				0, 0,
				nullptr);
			/// TODO:: Add a Fence ?
			imageList->Unbind();
			initializer->Unbind();
		}

		

	}
}

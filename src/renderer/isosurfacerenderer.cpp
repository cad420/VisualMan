
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
			camera({0.0,0.0,5}),width(w),height(h)
		{

		}

		void OITMeshRenderer::InitializeOpenGLResources()
		{
			InitShader();

			CreateMesh(R"(C:\Users\ysl\Desktop\dragon.obj)");

			proj.SetPerspective(45.0f, 800.0/600, 0.01, 100);


		}

		void OITMeshRenderer::RenderLoop()
		{
			// Set Camera Uniform

			testShader.bind();

			testShader.setUniformValue("model_matrix" ,model.Matrix());
			testShader.setUniformValue("view_matrix" , camera.view().Matrix());
			testShader.setUniformValue("projection_matrix" , proj.Matrix());
			testShader.setUniformValue("light_pos" , Vector3f{ 0.f,0.f,10.f });
			testShader.setUniformValue("light_color" , Vector3f{ 1.0,1.0,1.0 });
			testShader.setUniformValue("object_color" , RGBSpectrum{ 0.3 });
			testShader.setUniformValue("view_pos" , camera.position());

			for(auto & m:meshes)
			{			
				m.vao->bind();
				glDrawElements(GL_TRIANGLES, m.indexCount, GL_UNSIGNED_INT, 0);
			}
		}

		void OITMeshRenderer::CreateImageListTexture(int width, int height)
		{
			imageList = OpenGLTexture::CreateTexture2DRect(OpenGLTexture::R32UI,
				OpenGLTexture::FilterMode::Linear,
				OpenGLTexture::FilterMode::Linear,
				OpenGLTexture::WrapMode::ClampToEdge,
				OpenGLTexture::WrapMode::ClampToEdge,
				OpenGLTexture::RED, OpenGLTexture::UInt32, width, height, nullptr);

			//depthTexture = OpenGLTexture::CreateTexture2DRect(OpenGLTexture::InternalDepthComponent,
			//	OpenGLTexture::Linear,
			//	OpenGLTexture::Linear,
			//	OpenGLTexture::ClampToEdge,
			//	OpenGLTexture::ClampToEdge,
			//	OpenGLTexture::ExternalDepthComponent,
			//	OpenGLTexture::Float32,
			//	width, height,nullptr);

			//pbo = std::make_shared<OpenGLBuffer>(OpenGLBuffer::BufferTarget::PixelUnpackBuffer);

			//pbo->AllocateFor(nullptr, std::size_t(width) * std::size_t(height));
			//auto ptr = pbo->Map(OpenGLBuffer::WriteOnly);
			//memset(ptr, 0, std::size_t(width)*height);
			//pbo->Unmap();
		}

		void OITMeshRenderer::CreateFramebuffer()
		{
			//framebuffer = std::make_shared<OpenGLFramebufferObject>();

			//framebuffer->Bind();
			assert(imageList);

			//framebuffer->AttachTexture(OpenGLFramebufferObject::ColorAttachment0);
		}

		void OITMeshRenderer::CreateAtomicCounter()
		{
			atomicCounter = std::make_shared<OpenGLBuffer>(OpenGLBuffer::BufferTarget::AtomicCounterBuffer);

			ClearAtomicCounter();
		}


		void OITMeshRenderer::InitShader()
		{
			testShader.create();
			testShader.addShaderFromFile(R"(D:\code\MRE\src\shader\utils\phoneshadingvert.glsl)",ysl::ShaderProgram::ShaderType::Vertex);
			testShader.addShaderFromFile(R"(D:\code\MRE\src\shader\utils\phoneshadingfrag.glsl)",ysl::ShaderProgram::ShaderType::Fragment);
			testShader.link();
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

			const auto vertexSize = reader.vertexBytes();
			const auto normalSize = reader.normalBytes();
			const auto textureSize = reader.textureBytes();


			aMesh.vbo->Bind();
			aMesh.vbo->AllocateFor(nullptr, vertexSize + normalSize + textureSize);
			aMesh.vbo->SetSubData(reader.getVertices().data(),vertexSize, 0);
			aMesh.vbo->SetSubData(reader.getNormals().data(), normalSize, vertexSize);
			aMesh.vbo->SetSubData(reader.getTextureCoord().data(), textureSize, vertexSize + normalSize);

			// Vertex Attribute
			aMesh.vbo->VertexAttribPointer(0, 3, OpenGLBuffer::Float, false, sizeof(Point3f), nullptr);
			// Normal Attribute
			aMesh.vbo->VertexAttribPointer(1, 3, OpenGLBuffer::Float, false, sizeof(Vector3f), reinterpret_cast<void*>(vertexSize));
			// Texture Attribute
			aMesh.vbo->VertexAttribPointer(2, 2, OpenGLBuffer::Float, false, sizeof(Point2f), reinterpret_cast<void*>(vertexSize+normalSize));


			aMesh.ebo = std::make_shared<OpenGLBuffer>(OpenGLBuffer::BufferTarget::ElementArrayBuffer);
			aMesh.ebo->Bind();
			aMesh.indexCount = reader.getFaceIndices().size();
			aMesh.ebo->AllocateFor(reader.getFaceIndices().data(), reader.vertexIndicesBytes());
			meshes.push_back(aMesh);

		}

		void OITMeshRenderer::MousePressEvent(MouseEvent* event)
		{
			lastMousePos = event->pos();
		}

		void OITMeshRenderer::MouseMoveEvent(MouseEvent* event)
		{
			const auto & p = event->pos();
			//std::cout << p << std::endl;
			// Update Camera
			float dx = p.x - lastMousePos.x;
			float dy = lastMousePos.y - p.y;
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
			//std::cout << camera.view();
			lastMousePos = p;

		}

		void OITMeshRenderer::WindowResizeEvent(ResizeEvent* event)
		{
			const auto x = event->size().x;
			const auto y = event->size().y;
			//
			const auto aspect = static_cast<float>(x) / static_cast<float>(y);

			//std::cout << "aspect:" << aspect << std::endl;
			proj.SetPerspective(45.0f, aspect, 0.01, 100);

			UpdateImageListTexture(x, y);

		}

		void OITMeshRenderer::UpdateImageListTexture(int width, int height)
		{
			imageList->SetData(OpenGLTexture::R32UI, OpenGLTexture::RED, OpenGLTexture::UInt32, width, height, 0, nullptr);
		}

		void OITMeshRenderer::ClearAtomicCounter()
		{
			assert(atomicCounter);
			int zero = 0;
			atomicCounter->Bind();
			atomicCounter->AllocateFor(&zero, sizeof(int));
			atomicCounter->Unbind();

		}
	}
}

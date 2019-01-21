
#ifndef _ISOSURFACERENDERER_H_
#define _ISOSURFACERENDERER_H_
#include "../application/guiapplication.h"
#include "../opengl/shader.h"
#include "../opengl/openglbuffer.h"
#include "../opengl/openglvertexarrayobject.h"
#include "../cameras/camera.h"

namespace ysl
{
	namespace app
	{

		class OITMeshRenderer :public ImGuiApplication
		{
		public:
			OITMeshRenderer(int argc, char ** argv,int w,int h);
		protected:
			void InitializeOpenGLResources() override;
			void RenderLoop() override;

			void CreateImageListTexture(int width, int height);
			//void CreatePBO(int width,int height);
			void CreateFramebuffer();
			void CreateAtomicCounter();
			void CreateFragmentList();
			void InitShader();
			void SetShaderUniform();
			void CreateMesh(const std::string & fileName);

			void MousePressEvent(MouseEvent* event) override;
			void MouseMoveEvent(MouseEvent* event) override;

			void WindowResizeEvent(ResizeEvent* event) override;
			//void MouseReleaseEvent(MouseEvent* event) override;

		private:

			void UpdateImageListTexture(int width, int height);
			void ClearAtomicCounter();
			void ClearImageListTexture();

			int width;
			int height;

			FocusCamera camera;
			ShaderProgram vertShader;
			ShaderProgram fragShader;

			ShaderProgram testShader;

			ShaderProgram oitFragShader;
			ShaderProgram quadShader;

			Transform model;
			Transform proj;

			Point2i lastMousePos;

			std::shared_ptr<OpenGLTexture> imageList;
			std::shared_ptr<OpenGLTexture> depthTexture;
			//std::shared_ptr<OpenGLBuffer> pbo;
			std::shared_ptr<OpenGLBuffer> atomicCounter;
			std::shared_ptr<OpenGLBuffer> fragmentBufferList;

			std::shared_ptr<OpenGLFramebufferObject> framebuffer;

			struct Mesh
			{
				std::shared_ptr<OpenGLBuffer> vbo;
				std::shared_ptr<OpenGLBuffer> ebo;
				std::shared_ptr<OpenGLVertexArrayObject> vao;
				RGBASpectrum color;
				int indexCount;
			};
			std::vector<Mesh> meshes;

		};
	}
}

#endif

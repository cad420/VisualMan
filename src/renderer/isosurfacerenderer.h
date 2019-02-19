
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

			void MousePressEvent(MouseEvent* event) override;

			void MouseMoveEvent(MouseEvent* event) override;

			void WindowResizeEvent(ResizeEvent* event) override;
			//void MouseReleaseEvent(MouseEvent* event) override;
		private:

			void CreateScreenQuads();
			void CreateMesh(const std::string & fileName);

			void CreateImageListTexture(int width, int height);
			std::shared_ptr<OpenGLTexture> fragmentBufferListTexture;
			

			void CreateFragmentBufferList(int width, int height);
			std::shared_ptr<OpenGLBuffer> fragmentBufferListBuffer;

			void CreateHeadPointerImageInitializer(int width, int height);
			std::shared_ptr<OpenGLBuffer> initializer;	// a PBO 


			void CreateAtomicCounter();
			void ClearAtomicCounter();
			std::shared_ptr<OpenGLBuffer> atomicCounter;


			void InitShader();
			void SetShaderUniform();
			ShaderProgram testShader;
			ShaderProgram oitListShader;
			ShaderProgram oitRenderShader;
			ShaderProgram oitFragShader;
			ShaderProgram quadShader;


			void ResizeHeadPointerImage(int width, int height);

			void UpdateMatrix(int width, int height);

			void ResizeScreenQuads(int width, int height);

			void ResizeFragmentBufferList(int width, int height);

			void ResizeInitializer(int width, int height);

			void ClearHeadPointerImage();

			//void ClearImageListTexture();
			int windowWidth;
			int windowHeight;
			FocusCamera camera;
			Transform model;
			Transform proj;
			Transform ortho;
			Point2i lastMousePos;
			std::shared_ptr<OpenGLTexture> imageList;
			std::shared_ptr<OpenGLTexture> depthTexture;


			std::shared_ptr<OpenGLFramebufferObject> framebuffer;

			struct Mesh
			{
				std::shared_ptr<OpenGLBuffer> vbo;
				std::shared_ptr<OpenGLBuffer> ebo;
				std::shared_ptr<OpenGLVertexArrayObject> vao;
				RGBASpectrum color;
				int indexCount;
			};

			struct ScreenQuads 
			{
				OpenGLVertexArrayObject vao;
				std::shared_ptr<OpenGLBuffer> vbo;
			} screenQuads;
			std::vector<Mesh> meshes;
		};
	}
}

#endif

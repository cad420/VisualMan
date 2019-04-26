
#ifndef _OPENGLAPPLICATION_H_
#define _OPENGLAPPLICATION_H_

#include "application.h"
#include <memory>
#include "../opengl/framebuffer.h"

//#ifdef App
//#undef App
//#endif
//
//#define App (static_cast<GLFWApplication*>(Application::Instance()))

namespace ysl
{
	namespace app
	{
		class GLFWApplication :public Application
		{

		public:
			DISABLE_COPY(GLFWApplication);
			DISABLE_MOVE(GLFWApplication);
			~GLFWApplication();
			GLFWApplication(int argc, char ** argv, int w, int h);

			void SetClearColor(float* color);
			int Exec() override;
		protected:
			virtual void RenderLoop() = 0;
			virtual void InitializeOpenGLResources() = 0;
			void InitOpenGLContext();
			void DestroyOpenGLContext();
			void MousePressEvent(MouseEvent* e) override;
			void MouseReleaseEvent(MouseEvent* e) override;
			void MouseMoveEvent(MouseEvent* e) override;
			void WindowResizeEvent(ResizeEvent* event) override;
			void MouseWheelEvent(WheelEvent* e) override;

			GLFWwindow * window;
			int width;
			int height;
			bool mouseRightButtonPressed;
			bool mouseLeftButtonPressed;
			float clearColor[4];

		private:
			friend void glfwCursorPosCallback(GLFWwindow* window, double xpos, double ypos);
			friend void glfwMouseButtonCallback(GLFWwindow * window, int button, int action, int mods);
			friend void glfwWindowSizeCallback(GLFWwindow * window, int width, int height);
			friend void glfwMouseScrollCallback(GLFWwindow * window, double xoffset, double yoffset);
		};

	}

}



#endif

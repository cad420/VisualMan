

#include "openglapplication.h"
#include "../utility/error.h"
#include "../lib/gl3w/GL/gl3w.h"


static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

namespace ysl
{
	namespace app
	{
		OpenGLApplication::OpenGLApplication(int argc, char** argv, int w, int h) :
			Application(argc, argv),
			width(w),
			height(h),
			window(nullptr)
		{
			clearColor[0] = 0.45f;
			clearColor[1] = 0.55f;
			clearColor[2] = 0.60f;
			clearColor[3] = 1.00f;
		}

		void OpenGLApplication::SetClearColor(float *color[4])
		{
			memcpy(clearColor, color, sizeof(color));
		}
		int OpenGLApplication::Exec()
		{

			InitOpenGLContext();
			glfwMakeContextCurrent(window);
			InitializeOpenGLResources();
			while (!glfwWindowShouldClose(window))
			{
				glfwPollEvents();
				int display_w, display_h;
				glfwMakeContextCurrent(window);
				glfwGetFramebufferSize(window, &display_w, &display_h);
				glViewport(0, 0, display_w, display_h);
				glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
				glClear(GL_COLOR_BUFFER_BIT);



				RenderLoop();
				glfwSwapBuffers(window);
			}

			DestroyOpenGLContext();
			return 0;
		}

		void OpenGLApplication::InitOpenGLContext()
		{
			glfwSetErrorCallback(glfw_error_callback);
			if (!glfwInit())
				Error("GLFW cannot be initialized");
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
			window = glfwCreateWindow(width, height, "Mixed Render Engine", NULL, NULL);

			glfwMakeContextCurrent(window);

			glfwSwapInterval(1); // Enable vsync

			gl3wInit();
			if (!gl3wIsSupported(4, 4))
			{
				Error("OpenGL 4.4 or higher is needed.");
			}
		}
		void OpenGLApplication::DestroyOpenGLContext()
		{
			glfwDestroyWindow(window);
			glfwTerminate();
		}

		//void OpenGLApplication::MousePressEvent(MouseEvent* e)
		//{

		//}

		//void OpenGLApplication::MouseReleaseEvent(MouseEvent* e)
		//{

		//}

		//void OpenGLApplication::MouseMoveEvent(MouseEvent* e)
		//{

		//}
	}
	
}



#include "glfwapplication.h"
#include "../utility/error.h"
#include "../../lib/gl3w/GL/gl3w.h"

#include "event.h"


namespace ysl
{
	namespace app
	{

		static void glfw_error_callback(int error, const char* description)
		{
			fprintf(stderr, "Glfw Error %d: %s\n", error, description);
		}

		void MouseEventCheck(MouseEvent * e,const char * name)
		{
			std::string button;
			if (e->m_buttons & MouseEvent::LeftButton)
				button += "LeftButton ";
			if (e->m_buttons & MouseEvent::RightButton)
				button += "RightButton ";
			Log("%s [%d, %d]: Button:%s\n",name, e->m_pos.x, e->m_pos.y, button.c_str());
		}

		void glfwCursorPosCallback(GLFWwindow* window, double xpos, double ypos)
		{
			const auto app = App(GLFWApplication);

			assert(app->window == window);
			MouseEvent e({int(xpos),int(ypos)}, 0);
			if (app->mouseLeftButtonPressed)
				e.m_buttons |= MouseEvent::LeftButton;
			if (app->mouseRightButtonPressed)
				e.m_buttons |= MouseEvent::RightButton;
			if (e.m_buttons)
				app->MouseMoveEvent(&e);
		}

		void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
		{
			const auto app = App(GLFWApplication);
			assert(app->window == window);
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			MouseEvent e{ { int(xpos),int(ypos) }, 0 };
			if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
			{
				app->mouseRightButtonPressed = true;
				e.m_buttons |= MouseEvent::RightButton;
			}
			if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
			{
				app->mouseRightButtonPressed = false;
				e.m_buttons |= MouseEvent::RightButton;
			}
			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
			{
				app->mouseLeftButtonPressed = true;
				e.m_buttons |= MouseEvent::LeftButton;
			}
			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
			{
				app->mouseLeftButtonPressed = false;
				e.m_buttons |= MouseEvent::LeftButton;
			}
			if(e.buttons())
			{
				if (action == GLFW_PRESS)
					app->MousePressEvent(&e);
				else if (action == GLFW_RELEASE)
					app->MouseReleaseEvent(&e);
			}

		}

		void glfwWindowSizeCallback(GLFWwindow* window, int width, int height)
		{
			ResizeEvent e{ {width,height} };
			const auto app = App(GLFWApplication);
			assert(app->window == window);
			app->WindowResizeEvent(&e);
		}

		void glfwMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
		{
			const auto app = App(GLFWApplication);
			assert(app->window == window);	// Check if the context receiving the event is current context
			WheelEvent e(xoffset, yoffset);
			app->MouseWheelEvent(&e);
		}

		GLFWApplication::~GLFWApplication()
		{
			DestroyOpenGLContext();
		}

		GLFWApplication::GLFWApplication(int argc, char** argv, int w, int h) :
			Application(argc, argv),
			width(w),
			height(h),
			window(nullptr),
			mouseRightButtonPressed(false),
			mouseLeftButtonPressed(false)
		{
			clearColor[0] = 0.45f;
			clearColor[1] = 0.55f;
			clearColor[2] = 0.60f;
			clearColor[3] = 1.00f;
		}

		void GLFWApplication::SetClearColor(float *color[4])
		{
			memcpy(clearColor, color, sizeof(color));
		}

		int GLFWApplication::Exec()
		{

			InitOpenGLContext();
			glfwMakeContextCurrent(window);
			InitializeOpenGLResources();

			// Send Resize window Event
			WindowResizeEvent(&ResizeEvent{{width,height}});

			while (!glfwWindowShouldClose(window))
			{
				int display_w, display_h;
				glfwMakeContextCurrent(window);
				glfwGetFramebufferSize(window, &display_w, &display_h);
				glViewport(0, 0, display_w, display_h);
				glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
				glClear(GL_COLOR_BUFFER_BIT);
					RenderLoop();
				glfwPollEvents();
				glfwSwapBuffers(window);
			}
		
			return 0;
		}

		void GLFWApplication::InitOpenGLContext()
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

			// Add Input callback
			glfwSetWindowSizeCallback(window, glfwWindowSizeCallback);
			glfwSetCursorPosCallback(window,glfwCursorPosCallback);
			glfwSetMouseButtonCallback(window, glfwMouseButtonCallback);
			glfwSetScrollCallback(window, glfwMouseScrollCallback);

		}
		void GLFWApplication::DestroyOpenGLContext()
		{
			glfwDestroyWindow(window);
			glfwTerminate();
		}

		void GLFWApplication::MousePressEvent(MouseEvent* e)
		{
			(void)e;
		}

		void GLFWApplication::MouseReleaseEvent(MouseEvent* e)
		{
			(void)e;
		}

		void GLFWApplication::MouseMoveEvent(MouseEvent* e)
		{
			(void)e;
		}

		void GLFWApplication::WindowResizeEvent(ResizeEvent * event)
		{
			(void)event;
		}

		void GLFWApplication::MouseWheelEvent(WheelEvent* e)
		{
			(void)e;
		}
	}
	
}

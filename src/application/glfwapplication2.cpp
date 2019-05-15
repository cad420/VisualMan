
#include "glfwapplication2.h"
#include "../../lib/gl3w/GL/gl3w.h"
#include "../utility/error.h"
#include "event.h"


namespace ysl {
	namespace app
	{

		GLFWApplication2* GLFWApplication2::singleton = nullptr;
		std::thread::id GLFWApplication2::threadId;
		std::mutex GLFWApplication2::mutex;

		bool GLFWApplication2::InitWindow(const std::string& title, const graphics::RenderContextFormat& format, int width,
			int height)
		{
			DestroyWindow();

			if (!glfwInit())
				Error("GLFW cannot be initialized");


			glfwSetErrorCallback(glfw_error_callback);

			// Set context format
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			glfwWindowHint(GLFW_RED_BITS, format.GetRGBABits().x ? format.GetRGBABits().x : mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, format.GetRGBABits().y ? format.GetRGBABits().y : mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, format.GetRGBABits().z ? format.GetRGBABits().z : mode->blueBits);
			glfwWindowHint(GLFW_ALPHA_BITS, format.GetRGBABits().w ? format.GetRGBABits().w : GLFW_DONT_CARE);

			glfwWindowHint(GLFW_DEPTH_BITS, format.GetDepthBufferBits() ? format.GetDepthBufferBits() : GLFW_DONT_CARE);
			glfwWindowHint(GLFW_STENCIL_BITS, format.GetStencilBufferBits() ? format.GetStencilBufferBits() : GLFW_DONT_CARE);

			glfwWindowHint(GLFW_ACCUM_RED_BITS, format.GetAccumRGBABits().x ? format.GetAccumRGBABits().x : GLFW_DONT_CARE);
			glfwWindowHint(GLFW_ACCUM_GREEN_BITS, format.GetAccumRGBABits().y ? format.GetAccumRGBABits().y : GLFW_DONT_CARE);
			glfwWindowHint(GLFW_ACCUM_BLUE_BITS, format.GetAccumRGBABits().z ? format.GetAccumRGBABits().z : GLFW_DONT_CARE);
			glfwWindowHint(GLFW_ACCUM_ALPHA_BITS, format.GetAccumRGBABits().w? format.GetAccumRGBABits().w : GLFW_DONT_CARE);
			glfwWindowHint(GLFW_AUX_BUFFERS, GLFW_DONT_CARE);

			//glfwWindowHint(GLFW_STEREO, info.stereo());
			glfwWindowHint(GLFW_SAMPLES, format.MultiSample() ? format.GetMultiSampleNumber() : 0);
			glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_DONT_CARE);
			glfwWindowHint(GLFW_DOUBLEBUFFER, format.HasDoubleBuffer());
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);


			if(format.Profile() != graphics::ContextProfile::Core)
			{
				throw std::runtime_error("Only support OpenGL Core Profile\n");
			}

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

			glfwWindow = glfwCreateWindow(width,height, title.c_str(), NULL, NULL);

			glfwMakeContextCurrent(glfwWindow);

			InitContext();		// Init OpenGL Functions for context

			SetContextFormat(format);

			glfwSwapInterval(1); // Enable vsync

			// Add Input callback
			DispatchResizeEvent(width, height);

			glfwSetWindowSizeCallback(glfwWindow, glfwWindowSizeCallback);
			glfwSetCursorPosCallback(glfwWindow, glfwCursorPosCallback);
			glfwSetMouseButtonCallback(glfwWindow, glfwMouseButtonCallback);
			glfwSetScrollCallback(glfwWindow, glfwMouseScrollCallback);

			DispatchInitEvent();
		}

		void GLFWApplication2::DestroyWindow()
		{
			if(glfwWindow)
			{
				DispatchDestroyEvent();
				glfwDestroyWindow(glfwWindow);
				glfwWindow = nullptr;
			}
		}

		GLFWApplication2::GLFWApplication2(const std::string& title, const graphics::RenderContextFormat& format, int width,
			int height)
		{
			InitSingleton();
			InitWindow(title, format, width, height);
		}

		GLFWApplication2::~GLFWApplication2()
		{
			singleton = nullptr;
		}

		void GLFWApplication2::MakeCurrent()
		{
			assert(glfwWindow);
			glfwMakeContextCurrent(glfwWindow);
		}

		void GLFWApplication2::SwapBuffer()
		{
			assert(glfwWindow);
			glfwSwapBuffers(glfwWindow);
		}

		void GLFWApplication2::Update()
		{
			DispatchUpdateEvent();
		}

		int GLFWApplication2::Exec()
		{
			//DispatchResizeEvent(800, 600);
			while (!glfwWindowShouldClose(glfwWindow))
			{
				//glfwGetFramebufferSize(glfwWindow.get(), &display_w, &display_h);
				if (EnableUpdate())
					Update();
				glfwPollEvents();
				//glfwSwapBuffers(glfwWindow.get());
			}
			DestroyWindow();
			return 0;
		}


		void GLFWApplication2::glfwCursorPosCallback(GLFWwindow* window, double xpos, double ypos)
		{
			const auto ins = Instance();

			ins->glfwWindow == window;
			MouseEvent e({ int(xpos),int(ypos) }, 0);
			if (ins->mouseLeftButtonPressed)
				e.m_buttons |= MouseEvent::LeftButton;
			if (ins->mouseRightButtonPressed)
				e.m_buttons |= MouseEvent::RightButton;
			if (e.m_buttons)
				ins->DispatchMouseMoveEvent(graphics::Mouse_Left, xpos, ypos);
		}


		void GLFWApplication2::glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
		{
			const auto app = Instance();
			
			assert(app->glfwWindow == window);
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
			if (e.buttons())
			{
				if (action == GLFW_PRESS)
					app->DispatchMousePressedEvent(graphics::EMouseButton(graphics::Mouse_Left | graphics::Mouse_Right), xpos, ypos);
				else if (action == GLFW_RELEASE)
					app->DispatchMouseReleasedEvent(graphics::EMouseButton(graphics::Mouse_Left | graphics::Mouse_Right), xpos, ypos);
			}
		}

		void GLFWApplication2::glfwWindowSizeCallback(GLFWwindow* window, int width, int height)
		{
			ResizeEvent e{ {width,height} };
			const auto app = Instance();
			assert(app->glfwWindow == window);
			app->DispatchResizeEvent(width, height);
		}

		void GLFWApplication2::glfwMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
		{
			const auto app = Instance();
			assert(app->glfwWindow == window);	// Check if the context receiving the event is current context
			WheelEvent e(xoffset, yoffset);
			app->DispatchMouseWheelEvent(yoffset, xoffset);
		}

		void GLFWApplication2::glfw_error_callback(int error, const char* description)
		{
			fprintf(stderr, "Glfw Error %d: %s\n", error, description);
		}

		void GLFWApplication2::InitSingleton()
		{
			std::lock_guard<std::mutex> guard(mutex);
			if(singleton != nullptr)
			{
				if (threadId != std::this_thread::get_id())
					throw std::runtime_error("A GLFW window instance has been created from another thread");
				throw std::runtime_error("A GLFW window has been created\n");
			}
			threadId = std::this_thread::get_id();
			singleton = this;

		}

		void GLFWApplication2::Init()
		{

		}
	}
}

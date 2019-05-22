
#include "glfwapplication2.h"
#include "../../lib/gl3w/GL/gl3w.h"
#include "../utility/error.h"
#include "event.h"
#include "../graphic/abstraarray.h"
#include "../graphic/drawarray.h"
#include "../graphic/primitive.h"
#include "openglutils.h"
#include "../graphic/renderstudio.h"
#include "../graphic/trivialscenemanager.h"
#include "../graphic/actor.h"


namespace ysl {
	namespace app
	{

		GLFWApplication2* GLFWApplication2::singleton = nullptr;
		std::thread::id GLFWApplication2::threadId;
		std::mutex GLFWApplication2::mutex;


		//unsigned VAO;
		//Ref<graphics::GLSLProgram> glslProgram;
		//Ref<graphics::ArrayFloat3> triangle;
		Ref<vm::Primitive> primitive;
		Ref<vm::Frame> frame;

		bool GLFWApplication2::InitWindow(const std::string& title, const vm::RenderContextFormat& format, int width,
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
			glfwWindowHint(GLFW_ACCUM_ALPHA_BITS, format.GetAccumRGBABits().w ? format.GetAccumRGBABits().w : GLFW_DONT_CARE);
			glfwWindowHint(GLFW_AUX_BUFFERS, GLFW_DONT_CARE);

			//glfwWindowHint(GLFW_STEREO, info.stereo());
			glfwWindowHint(GLFW_SAMPLES, format.MultiSample() ? format.GetMultiSampleNumber() : 0);
			glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_DONT_CARE);
			glfwWindowHint(GLFW_DOUBLEBUFFER, format.HasDoubleBuffer());
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);


			if (format.Profile() != vm::ContextProfile::Core)
			{
				throw std::runtime_error("Only support OpenGL Core Profile\n");
			}

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

			glfwWindow = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

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
			if (glfwWindow)
			{
				DispatchDestroyEvent();
				glfwDestroyWindow(glfwWindow);
				glfwWindow = nullptr;
			}
		}

		GLFWApplication2::GLFWApplication2(const std::string& title,
			const vm::RenderContextFormat& format,
			int width,
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

		int GLFWApplication2::Show()
		{
			DispatchResizeEvent(800, 600);
			while (!glfwWindowShouldClose(glfwWindow))
			{
				if (EnableUpdate())
					Update();
				glfwPollEvents();
			}
			DestroyWindow();
			return 0;
		}


		void GLFWApplication2::glfwCursorPosCallback(GLFWwindow* window, double xpos, double ypos)
		{
			const auto ins = Instance();

			ins->glfwWindow == window;
			
			int button = 0;
			if (ins->mouseLeftButtonPressed)
				button |= vm::Mouse_Left;
			if (ins->mouseRightButtonPressed)
				button |= vm::Mouse_Right;
			if (button)
				ins->DispatchMouseMoveEvent((vm::EMouseButton)button, xpos, ypos);
		}


		void GLFWApplication2::glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
		{
			const auto app = Instance();

			assert(app->glfwWindow == window);
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			int buttons = 0;
			if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
			{
				app->mouseRightButtonPressed = true;
				buttons |= vm::Mouse_Right;
			}
			if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
			{
				app->mouseRightButtonPressed = false;
				buttons |= vm::Mouse_Right;
			}
			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
			{
				app->mouseLeftButtonPressed = true;
				buttons |= vm::Mouse_Left;
			}
			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
			{
				app->mouseLeftButtonPressed = false;
				buttons |= vm::Mouse_Left;
			}
			if (buttons)
			{
				if (action == GLFW_PRESS)
					app->DispatchMousePressedEvent((vm::EMouseButton)buttons, xpos, ypos);
				else if (action == GLFW_RELEASE)
					app->DispatchMouseReleasedEvent((vm::EMouseButton)buttons, xpos, ypos);
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
			if (singleton != nullptr)
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

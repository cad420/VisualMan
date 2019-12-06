
#include "vmglfwwindow.h"
#include <abstraarray.h>
#include <primitive.h>
#include <renderstudio.h>
#include <thread>


namespace ysl {
	namespace app
	{

		VMGLFWWindow* VMGLFWWindow::singleton = nullptr;
		std::thread::id VMGLFWWindow::threadId;
		std::mutex VMGLFWWindow::mutex;
		Ref<vm::Primitive> primitive;
		Ref<vm::Aggregate> frame;

		bool VMGLFWWindow::InitWindow(const std::string& title, const vm::RenderContextFormat& format, int width,
			int height)
		{
			DestroyWindow();

			if (!glfwInit())
			{
				std::cout << "GLFW cannot be initialized" << std::endl;
				return false;
			}

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


			/*Window size need to be delivered before initialization
			 * process because many initialization depend the window size*/
			DispatchResizeEvent(width,height);


			glfwSetFramebufferSizeCallback(glfwWindow, glfwFramebufferSizeCallback);
			glfwSetCursorPosCallback(glfwWindow, glfwCursorPosCallback);
			glfwSetMouseButtonCallback(glfwWindow, glfwMouseButtonCallback);
			glfwSetScrollCallback(glfwWindow, glfwMouseScrollCallback);
			glfwSetKeyCallback(glfwWindow, glfwKeyCallback);
			glfwSetDropCallback(glfwWindow, glfwDropFileCallback);

			DispatchInitEvent();
			glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			return true;
		}

		void VMGLFWWindow::DestroyWindow()
		{
			if (glfwWindow)
			{
				DispatchDestroyEvent();
				glfwDestroyWindow(glfwWindow);
				glfwWindow = nullptr;
			}
		}

		VMGLFWWindow::VMGLFWWindow(const std::string& title,
			const vm::RenderContextFormat& format,
			int width,
			int height):width(width),height(height),RenderContext(width,height)
		{
			InitSingleton();
			InitWindow(title, format, width, height);
		}

		VMGLFWWindow::~VMGLFWWindow()
		{
			singleton = nullptr;
			DestroyWindow();
		}

		void VMGLFWWindow::MakeCurrent()
		{
			assert(glfwWindow);
			glfwMakeContextCurrent(glfwWindow);
		}

		void VMGLFWWindow::SwapBuffer()
		{
			assert(glfwWindow);
			glfwSwapBuffers(glfwWindow);
		}

		void VMGLFWWindow::Update()
		{
			DispatchUpdateEvent();
		}

		void VMGLFWWindow::SetWindowTitle(const std::string& title)
		{
			if(glfwWindow)
			{
				glfwSetWindowTitle(glfwWindow, title.c_str());
			}
		}

		int VMGLFWWindow::Show()
		{
			DispatchResizeEvent(width, height);
			while (!glfwWindowShouldClose(glfwWindow) && IsTerminate() == false)
			{
				if (GetAutomaticallyUpdate())
					Update();
				glfwPollEvents();
			}
			DestroyWindow();
			return 0;
		}

		void VMGLFWWindow::glfwCursorPosCallback(GLFWwindow* window, double xpos, double ypos)
		{
			const auto ins = Instance();
			ins->glfwWindow == window;

			int button = 0;
			if (ins->mouseLeftButtonPressed)
				button |= vm::Mouse_Left;
			if (ins->mouseRightButtonPressed)
				button |= vm::Mouse_Right;
			if (button)
				ins->DispatchMouseMoveEvent((vm::MouseButton)button, xpos, ypos);
		}

		void VMGLFWWindow::glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
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
					app->DispatchMousePressedEvent((vm::MouseButton)buttons, xpos, ypos);
				else if (action == GLFW_RELEASE)
					app->DispatchMouseReleasedEvent((vm::MouseButton)buttons, xpos, ypos);
			}
		}

		void VMGLFWWindow::glfwFramebufferSizeCallback(GLFWwindow* window, int width, int height)
		{
			//ResizeEvent e{ {width,height} };
			const auto app = Instance();
			assert(app->glfwWindow == window);
			app->DispatchResizeEvent(width, height);
		}

		void VMGLFWWindow::glfwMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
		{
			const auto app = Instance();
			assert(app->glfwWindow == window);	// Check if the context receiving the event is current context
			//WheelEvent e(xoffset, yoffset);
			app->DispatchMouseWheelEvent(yoffset, xoffset);
		}

		void VMGLFWWindow::glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			const auto app = Instance();
			assert(app->glfwWindow == window);
			switch (action)
			{
			case GLFW_REPEAT:
			case GLFW_PRESS:app->DispatchKeyPressedEvent(TranslateKey(key, scancode, mods)); break;
			case GLFW_RELEASE:app->DispatchKeyReleasedEvent(TranslateKey(key, scancode, mods)); break;
			default:std::cout << "Unsupported callback\n";
			}
		}

		void VMGLFWWindow::glfwDropFileCallback(GLFWwindow* window, int count, const char** df)
		{
			const auto app = Instance();
			assert(app->glfwWindow == window);
			std::vector<std::string> fileNames;
			for (int i = 0; i < count; i++)
			{
				fileNames.emplace_back(df[i]);
			}
			app->DispatchFileDropEvent(fileNames);
		}

		void VMGLFWWindow::glfw_error_callback(int error, const char* description)
		{
			fprintf(stderr, "Glfw Error %d: %s\n", error, description);
		}


	}

	vm::KeyButton app::VMGLFWWindow::TranslateKey(int key, int scancode, int mods)
	{
		switch (key) {
		case GLFW_KEY_0: return vm::Key_0;
		case GLFW_KEY_1: return vm::Key_1;
		case GLFW_KEY_2: return vm::Key_2;
		case GLFW_KEY_3: return vm::Key_3;
		case GLFW_KEY_4: return vm::Key_4;
		case GLFW_KEY_5: return vm::Key_5;
		case GLFW_KEY_6: return vm::Key_6;
		case GLFW_KEY_7: return vm::Key_7;
		case GLFW_KEY_8: return vm::Key_8;
		case GLFW_KEY_9: return vm::Key_9;
		case GLFW_KEY_A: return vm::Key_A;
		case GLFW_KEY_B: return vm::Key_B;
		case GLFW_KEY_C: return vm::Key_C;
		case GLFW_KEY_D: return vm::Key_D;
		case GLFW_KEY_E: return vm::Key_E;
		case GLFW_KEY_F: return vm::Key_F;
		case GLFW_KEY_G: return vm::Key_G;
		case GLFW_KEY_H: return vm::Key_H;
		case GLFW_KEY_I: return vm::Key_I;
		case GLFW_KEY_J: return vm::Key_J;
		case GLFW_KEY_K: return vm::Key_K;
		case GLFW_KEY_L: return vm::Key_L;
		case GLFW_KEY_M: return vm::Key_M;
		case GLFW_KEY_N: return vm::Key_N;
		case GLFW_KEY_O: return vm::Key_O;
		case GLFW_KEY_P: return vm::Key_P;
		case GLFW_KEY_Q: return vm::Key_Q;
		case GLFW_KEY_R: return vm::Key_R;
		case GLFW_KEY_S: return vm::Key_S;
		case GLFW_KEY_T: return vm::Key_T;
		case GLFW_KEY_U: return vm::Key_U;
		case GLFW_KEY_V: return vm::Key_V;
		case GLFW_KEY_W: return vm::Key_W;
		case GLFW_KEY_X: return vm::Key_X;
		case GLFW_KEY_Y: return vm::Key_Y;
		case GLFW_KEY_Z: return vm::Key_Z;

		case GLFW_KEY_RIGHT: return vm::Key_Right;
		case GLFW_KEY_LEFT: return vm::Key_Left;
		case GLFW_KEY_DOWN: return vm::Key_Down;
		case GLFW_KEY_UP: return vm::Key_Up;

		case GLFW_KEY_KP_0: return vm::Key_0;
		case GLFW_KEY_KP_1: return vm::Key_1;
		case GLFW_KEY_KP_2: return vm::Key_2;
		case GLFW_KEY_KP_3: return vm::Key_3;
		case GLFW_KEY_KP_4: return vm::Key_4;
		case GLFW_KEY_KP_5: return vm::Key_5;
		case GLFW_KEY_KP_6: return vm::Key_6;
		case GLFW_KEY_KP_7: return vm::Key_7;
		case GLFW_KEY_KP_8: return vm::Key_8;
		case GLFW_KEY_KP_9: return vm::Key_9;
		default:std::cout << "Unsupported key\n";
		}

		//case GLFW_KEY_RIGHT_SUPER:
		// case GLFW_KEY_MENU:
	}

	void app::VMGLFWWindow::InitSingleton()
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

	void app::VMGLFWWindow::Init()
	{

	}

}

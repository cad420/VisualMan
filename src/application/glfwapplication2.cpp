
#include "glfwapplication2.h"
#include "../../lib/gl3w/GL/gl3w.h"
#include "../utility/error.h"
#include "event.h"
#include "../graphic/abstraarray.h"


namespace ysl {
	namespace app
	{

		GLFWApplication2* GLFWApplication2::singleton = nullptr;
		std::thread::id GLFWApplication2::threadId;
		std::mutex GLFWApplication2::mutex;


		unsigned VAO;
		Ref<graphics::GLSLProgram> glslProgram;

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
			glfwWindowHint(GLFW_ACCUM_ALPHA_BITS, format.GetAccumRGBABits().w ? format.GetAccumRGBABits().w : GLFW_DONT_CARE);
			glfwWindowHint(GLFW_AUX_BUFFERS, GLFW_DONT_CARE);

			//glfwWindowHint(GLFW_STEREO, info.stereo());
			glfwWindowHint(GLFW_SAMPLES, format.MultiSample() ? format.GetMultiSampleNumber() : 0);
			glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_DONT_CARE);
			glfwWindowHint(GLFW_DOUBLEBUFFER, format.HasDoubleBuffer());
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);


			if (format.Profile() != graphics::ContextProfile::Core)
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


			// ---------TEST CODE---------------------------------

			const unsigned int SCR_WIDTH = 800;
			const unsigned int SCR_HEIGHT = 600;

			const char *vertexShaderSource = "#version 330 core\n"
				"layout (location = 0) in vec3 aPos;\n"
				"void main()\n"
				"{\n"
				"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
				"}\0";
			const char *fragmentShaderSource = "#version 330 core\n"
				"out vec4 FragColor;\n"
				"void main()\n"
				"{\n"
				"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
				"}\n\0";

			// build and compile our shader program
			// ------------------------------------
			// vertex shader


			Ref<graphics::GLSLProgram> program;

			auto  vertShader = MakeRef<graphics::GLSLShader>(ST_VERTEX_SHADER);
			vertShader->SetFromSource(vertexShaderSource);
			//vertShader->CreateShader();
			assert(vertShader->Compile());

			auto fragShader = MakeRef<graphics::GLSLShader>(ST_FRAGMENT_SHADER);
			fragShader->SetFromSource(fragmentShaderSource);
			//fragShader->CreateShader();
			assert(fragShader->Compile());

			glslProgram = MakeRef<graphics::GLSLProgram>();

			glslProgram->AttachShader(vertShader);
			glslProgram->AttachShader(fragShader);
			glslProgram->DetachShader(fragShader);
			//glslProgram->Link();
			//assert(glslProgram->Linked());



			//int vertexShader = glCreateShader(GL_VERTEX_SHADER);
			//glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
			//glCompileShader(vertexShader);
			//// check for shader compile errors
			//int success;
			//char infoLog[512];
			//glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
			//if (!success)
			//{
			//	glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			//	std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			//}
			//// fragment shader
			//int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			//glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
			//glCompileShader(fragmentShader);
			//// check for shader compile errors
			//glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
			//if (!success)
			//{
			//	glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			//	std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
			//}
			//// link shaders
			//shaderProgram = glCreateProgram();
			//glAttachShader(shaderProgram, vertexShader);
			//glAttachShader(shaderProgram, fragmentShader);
			//glLinkProgram(shaderProgram);
			//// check for linking errors
			//glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
			//if (!success) {
			//	glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			//	std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
			//}
			//glDeleteShader(vertexShader);
			//glDeleteShader(fragmentShader);

			// set up vertex data (and buffer(s)) and configure vertex attributes
			// ------------------------------------------------------------------
			float vertices[] = {
				 0.5f,  0.5f, 0.0f,  // top right
				 0.5f, -0.5f, 0.0f,  // bottom right
				-0.5f, -0.5f, 0.0f,  // bottom left
				-0.5f,  0.5f, 0.0f   // top left 
			};
			unsigned int indices[] = {  // note that we start from 0!
				0, 1, 3,  // first Triangle
				1, 2, 3   // second Triangle
			};


			//graphics::ArrayFloat3 triangle;

			auto triangle = MakeRef<graphics::ArrayFloat3>();
			auto bo = triangle->GetBufferObject();
			bo->SetBufferData(sizeof(vertices), vertices, BU_STATIC_DRAW);




			//unsigned int VAO;
			unsigned int  VBO, EBO;
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);
			// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
			//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
			// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
			glBindVertexArray(0);

			// ----------------TEST CODE END--------------------------

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
			const graphics::RenderContextFormat& format,
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


			// -----------TEST CODE ---------------------------

			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			// draw our first triangle
			//glUseProgram(shaderProgram);
			glslProgram->Apply(-1, nullptr, this);
			glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
			//glDrawArrays(GL_TRIANGLES, 0, 6);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			// ------------TEST CODE--------------------------

			glfwSwapBuffers(glfwWindow);

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

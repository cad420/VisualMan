
#ifndef _GLFWAPP2_H_
#define _GLFWAPP2_H_
#include "../graphic/rendercontext.h"
#include "../../lib/glfw/include/GLFW/glfw3.h"
#include <mutex>

namespace ysl
{
	namespace app
	{
		class GLFWApplication2:public vm::RenderContext
		{
		public:
			GLFWApplication2(const std::string& title, const vm::RenderContextFormat& format, int width,
				int height);

			~GLFWApplication2();

			bool InitWindow(const std::string & title, const vm::RenderContextFormat & format,int width,int height);

			void DestroyWindow();

			void MakeCurrent()override;

			void SwapBuffer()override;

			void Update()override;

			int Show();

			const GLFWwindow * Handle() const { return glfwWindow; }

			static GLFWApplication2 * Instance() { return singleton; }
		private:
			
			static void glfwCursorPosCallback(GLFWwindow* window, double xpos, double ypos);
			static void glfwMouseButtonCallback(GLFWwindow * window, int button, int action, int mods);
			static void glfwWindowSizeCallback(GLFWwindow * window, int width, int height);
			static void glfwMouseScrollCallback(GLFWwindow * window, double xoffset, double yoffset);
			static void glfw_error_callback(int error, const char* description);

			void InitSingleton();
			void Init();

			static GLFWApplication2* singleton;
			static std::thread::id threadId;
			static std::mutex mutex;
			GLFWwindow * glfwWindow = nullptr;
			bool mouseRightButtonPressed = false;
			bool mouseLeftButtonPressed = false;

		};
	}
}

#endif

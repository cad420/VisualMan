
#ifndef _VMGLFWWINDOW_H_
#define _VMGLFWWINDOW_H_
#include <rendercontext.h>
#include <GLFW/glfw3.h>
#include <mutex>
#include <thread>

#if defined(_WIN32) && defined(VM_SHARED_LIBRARY)
#ifdef vmglfwwindow_EXPORTS
#define GLFWWINDOW_EXPORT_IMPORT __declspec(dllexport)
#else
#define GLFWWINDOW_EXPORT_IMPORT __declspec(dllimport)
#endif
#else
#define GLFWWINDOW_EXPORT_IMPORT
#endif

namespace ysl
{
	namespace app
	{
		class GLFWWINDOW_EXPORT_IMPORT VMGLFWWindow:public vm::RenderContext
		{
		public:
			VMGLFWWindow(const std::string& title, const vm::RenderContextFormat& format, int width,
				int height);

			~VMGLFWWindow();

			bool InitWindow(const std::string & title, const vm::RenderContextFormat & format,int width,int height);

			void DestroyWindow();

			void MakeCurrent()override;

			void SwapBuffer()override;

			void Update()override;

			void SetWindowTitle(const std::string& title) override;

			int Show();

			const GLFWwindow * Handle() const { return glfwWindow; }

			static	VMGLFWWindow * Instance() { return singleton; }
		private:
			
			static void glfwCursorPosCallback(GLFWwindow* window, double xpos, double ypos);
			static void glfwMouseButtonCallback(GLFWwindow * window, int button, int action, int mods);
			static void glfwFramebufferSizeCallback(GLFWwindow * window, int width, int height);
			static void glfwMouseScrollCallback(GLFWwindow * window, double xoffset, double yoffset);
			static void glfwKeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);
			static void glfwDropFileCallback(GLFWwindow * window,int count ,const char **df);
			static void glfw_error_callback(int error, const char* description);
			static vm::KeyButton TranslateKey(int key, int scancode, int mods);
			void InitSingleton();
			void Init();

			static VMGLFWWindow* singleton;
			static std::thread::id threadId;
			static std::mutex mutex;

			GLFWwindow * glfwWindow = nullptr;
			bool mouseRightButtonPressed = false;
			bool mouseLeftButtonPressed = false;
			int width = 800;
			int height = 600;

			//static std::unordered_map<GLFWwindow *, VMGLFWWindow*> windowMap;

		};
	}
}

#endif

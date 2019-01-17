
#include "guiapplication.h"
#include "../../lib/glfw/include/GLFW/glfw3.h"
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_impl_glfw.h"
#include "../../imgui/imgui_impl_opengl3.h"
#include "GL/gl3w.h"
#include "error.h"

namespace ysl
{
	namespace app
	{
		ImGuiApplication::ImGuiApplication(int argc, char** argv, int w, int h) :
			GLFWApplication(argc, argv, w, h)
		{

		}

		void ImGuiApplication::AddWidget(const std::shared_ptr<imgui::ImGuiInterface>& widget)
		{
			widgets.push_back(widget);
		}

		int ImGuiApplication::Exec()
		{
			InitOpenGLContext();

			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGui_ImplGlfw_InitForOpenGL(window, false);  // Use our default GLFW callbacks
			ImGui_ImplOpenGL3_Init();
			// Setup style
			//ImGui::StyleColorsDark();
			//ImGui::StyleColorsLight();
			ImGui::StyleColorsClassic();
			InitializeOpenGLResources();
			while (!glfwWindowShouldClose(window))
			{
				glfwPollEvents();
				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();

				DrawImGui();

				ImGui::EndFrame();
				// Rendering
				ImGui::Render();

				int display_w, display_h;
				glfwMakeContextCurrent(window);
				glfwGetFramebufferSize(window, &display_w, &display_h);
				glViewport(0, 0, display_w, display_h);
				glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
				glClear(GL_COLOR_BUFFER_BIT);
				RenderLoop();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
				glfwSwapBuffers(window);
			}
			DestroyOpenGLContext();
			return 0;
		}

		void ImGuiApplication::DrawImGui()
		{

		}
	}
}

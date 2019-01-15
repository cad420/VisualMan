
#include "guiapplication.h"
#include "../../lib/glfw/include/GLFW/glfw3.h"
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_impl_glfw.h"
#include "../../imgui/imgui_impl_opengl3.h"
#include "GL/gl3w.h"

namespace ysl
{
	namespace app
	{
		GuiApplication::GuiApplication(int argc, char** argv, int w, int h) :
			OpenGLApplication(argc, argv, w, h)
		{

		}

		void GuiApplication::AddWidget(const std::shared_ptr<imgui::ImGuiInterface>& widget)
		{
			widgets.push_back(widget);
		}

		int GuiApplication::Exec()
		{
			InitOpenGLContext();

			glfwMakeContextCurrent(window);

			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			ImGui_ImplGlfw_InitForOpenGL(window, true);
			ImGui_ImplOpenGL3_Init();

			// Setup style
			ImGui::StyleColorsDark();
			InitializeOpenGLResources();
			while (!glfwWindowShouldClose(window))
			{
				glfwPollEvents();
				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();

				for (auto & w : widgets)
					w->Draw();
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
	}
}

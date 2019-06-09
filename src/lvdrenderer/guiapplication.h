#ifndef _GUIAPPLICATION_H_
#define _GUIAPPLICATION_H_

#include "glfwapplication.h"
#include "imguibase.h"
#include <vector>
#include <memory>

namespace ysl
{
	namespace app
	{
		class ImGuiApplication : public GLFWApplication
		{
		public:
			ImGuiApplication(int argc, char ** argv, int w, int h);
			ImGuiApplication(const ImGuiApplication &) = delete;
			ImGuiApplication & operator=(const ImGuiApplication &) = delete;
			ImGuiApplication(ImGuiApplication &&) = delete;
			ImGuiApplication& operator=(ImGuiApplication&&) = delete;
			void AddWidget(const std::shared_ptr<imgui::ImGuiInterface> & widget);
			int Exec() override;
			~ImGuiApplication() = default;
		protected:
			virtual void DrawImGui();
		private:
			std::vector<std::shared_ptr<imgui::ImGuiInterface>> widgets;
		};

	}
}

#endif // !_GUIAPPLICATION_H_



#ifndef _GUIAPPLICATION_H_
#define _GUIAPPLICATION_H_
#include "openglapplication.h"
#include "../gui/imguibase.h"
#include <vector>


namespace ysl
{
	namespace app
	{
		class GuiApplication : public OpenGLApplication
		{
		public:
			GuiApplication(int argc, char ** argv, int w, int h);
			DISABLE_COPY(GuiApplication);
			DISABLE_MOVE(GuiApplication);
			void AddWidget(const std::shared_ptr<imgui::ImGuiInterface> & widget);
			int Exec() override;
			~GuiApplication() = default;
		private:
			std::vector<std::shared_ptr<imgui::ImGuiInterface>> widgets;
		};

	}
}

#endif // !_GUIAPPLICATION_H_

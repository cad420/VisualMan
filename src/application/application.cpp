
#include "application.h"

#include "../utility/error.h"

namespace ysl
{
	namespace app
	{

		Application * Application::app = nullptr;
		Application::Application(int argc, char** argv)
		{
			(void)argc;
			(void)argv;

			if (!app)
				app = this;
			else
			{
				Error("Application has been created\n");
			}
		}

		Application::~Application()
		{
			app = nullptr;
		}

		Application * Application::Instance()
		{
			return app;
		}
	}
}

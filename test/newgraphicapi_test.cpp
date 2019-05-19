#include "../src/application/glfwapplication2.h"
#include "../src/graphic/assembly.h"
#include "../src/graphic/graphicsbooster.h"

int main(int argc,char ** argv)
{
	using namespace ysl::graphics;
	using namespace ysl::app;

	ysl::graphics::Booster booster(argc, argv);


	GLFWApplication2 window("Test GLFW Window", RenderContextFormat(), 800, 600);

	auto app = MakeRef<Assembly>();

	assert(window.GetFramebuffer());
	//window.AddEventListener(app);

	//assert(app->Rendering()->Renderers().size());
	//app->Rendering()->Renderers()[0]->SetFramebuffer(window.GetFramebuffer());

	auto ret = window.Exec();

	system("pause");
	return ret;
}

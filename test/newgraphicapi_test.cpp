#include "../src/application/glfwapplication2.h"
#include "../src/graphic/assembly.h"
#include "../src/graphic/graphicsbooster.h"
#include "../src/graphic/vpl_mesh.h"

int main(int argc,char ** argv)
{
	using namespace ysl::vpl;
	using namespace ysl::app;

	Booster booster(argc, argv);
	GLFWApplication2 window("Test GLFW Window", RenderContextFormat(), 800, 600);
	auto app = MakeRef<VPL_Mesh>();
	assert(window.GetFramebuffer());


	window.AddEventListener(app);

	//app->InitDefault();

	app->Rendering()->Renderers()[0]->SetFramebuffer(window.GetFramebuffer());


	auto ret = window.Show();
	system("pause");
	return ret;
}

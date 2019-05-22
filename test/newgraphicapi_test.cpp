#include "../src/application/glfwapplication2.h"
#include "../src/graphic/visualman.h"
#include "../src/graphic/graphicsbooster.h"
#include "../src/graphic/vm_mesh.h"

int main(int argc,char ** argv)
{
	using namespace ysl::vm;
	using namespace ysl::app;

	Booster booster(argc, argv);
	GLFWApplication2 window("Test GLFW Window", RenderContextFormat(), 800, 600);
	auto appMesh = MakeRef<VM_Mesh>();
	//assert(window.GetFramebuffer());
	window.AddEventListener(appMesh);
	//app->InitDefault();

	appMesh->Rendering()->Renderers()[0]->SetFramebuffer(window.GetFramebuffer());

	auto ret = window.Show();
	system("pause");
	return ret;
}

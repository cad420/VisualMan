
#include <vm_volumeraycast.h>
#include <graphicsbooster.h>
#include "vmglfwwindow.h"

int main(int argc, char ** argv)
{
	using namespace ysl::vm;
	using namespace ysl::app;
	Booster booster(argc, argv);
	GLFWApplication2 window("Test GLFW Window", RenderContextFormat(), 800, 600);
	auto appMesh = MakeRef<VM_VolumeRayCast>();
	window.AddEventListener(appMesh);
	appMesh->GetAggregate()->Renderers()[0]->SetFramebuffer(window.GetFramebuffer());
	auto ret = window.Show();
	return ret;
}

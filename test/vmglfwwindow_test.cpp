
#include <vm_volumeraycast.h>
#include <graphicsbooster.h>
#include "vmglfwwindow.h"
#include "vm_mesh.h"
#include "vm_isosurface.h"
#include "../src/io/rawreader.h"

int main(int argc, char ** argv)
{
	using namespace ysl::vm;
	using namespace ysl::app;
	Booster booster(argc, argv);

	VMGLFWWindow window("Test GLFW Window", RenderContextFormat(), 800, 600);

	auto appMesh = MakeRef<VM_VolumeRayCast>();

	window.AddEventListener(appMesh);
	
	auto ret = window.Show();
	return ret;
}

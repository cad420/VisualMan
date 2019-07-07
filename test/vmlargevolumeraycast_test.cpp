
#include "vmglfwwindow.h"
#include "vm_largevolumeraycast.h"
#include "cmdline.h"


int main(int argc, char ** argv)
{
	using namespace ysl::vm;
	using namespace ysl::app;

	cmdline::parser a;
	a.add<int>("width", 'w', "The width of window", false, 1024);
	a.add<int>("height", 'h', "The height of window", false, 768);
	a.parse_check(argc, argv);
	VMGLFWWindow window("LVD Renderer", RenderContextFormat(), a.get<int>("width"), a.get<int>("height"));
	auto app = MakeRef<VM_LargeVolumeRayCast>();
	window.AddEventListener(app);
	return window.Show();
}

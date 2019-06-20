
#include "vmglfwwindow.h"
#include "vm_largevolumeraycast.h"
#include "cmdline.h"

int main(int argc,char ** argv)
{
	using namespace ysl::vm;
	using namespace ysl::app;

	cmdline::parser clp;
	clp.add<int>("width",'w',"The width of window",false,1024);
	clp.add<int>("height", 'h', "The height of window", false, 768);
	clp.parse_check(argc, argv);

	VMGLFWWindow window("LVD Renderer", RenderContextFormat(), clp.get<int>("width"),clp.get<int>("height"));
	auto app = MakeRef<VM_LargeVolumeRayCast>();
	window.AddEventListener(app);
	auto ret = window.Show();
	return ret;
}

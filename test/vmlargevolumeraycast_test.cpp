
#include "vmglfwwindow.h"
#include "vm_largevolumeraycast.h"
#include <VMUtils/cmdline.hpp>
#include <VMFoundation/pluginloader.h>

int main(int argc, char ** argv)
{
	using namespace ysl::vm;
	using namespace ysl::app;
	// Load plugins
	
	ysl::PluginLoader::GetPluginLoader()->LoadPlugins("plugins");
	cmdline::parser a;
	a.add<int>("width", 'w', "The width of window", false, 1024);
	a.add<int>("height", 'h', "The height of window", false, 768);

	a.add<int>("of", 'f', "output image file name", false);
	a.add("offline", 'o', "offline rendering");
	a.add<std::string>("data", 'd', "data configuration file", false);
	a.add<std::string>("tf", 't', "transfer function file", false);

	a.parse_check(argc, argv);

	VMGLFWWindow window("LVD Renderer", RenderContextFormat(), a.get<int>("width"), a.get<int>("height"));
	auto app = MakeRef<VM_LargeVolumeRayCast>();
	window.AddEventListener(app);
	auto r = window.Show();
	return r;
}

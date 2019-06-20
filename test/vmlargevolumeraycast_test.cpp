
#include "vmglfwwindow.h"
#include "vm_largevolumeraycast.h"

int main(int argc,char ** argv)
{
	using namespace ysl::vm;
	using namespace ysl::app;
	VMGLFWWindow window("LVD Renderer", RenderContextFormat(), 1280,960);
	auto app = MakeRef<VM_LargeVolumeRayCast>();
	window.AddEventListener(app);
	auto ret = window.Show();
	return ret;
}

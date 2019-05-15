#include "../src/application/glfwapplication2.h"
#include "../src/graphic/assembly.h"

int main(int argc,char ** argv)
{
	using namespace ysl::graphics;
	using namespace ysl::app;
	GLFWApplication2 app("Test GLFW Window", RenderContextFormat(), 800, 600);
	app.AddUIEventListener(Ref<IEventListener>(new Assembly));
	auto ret = app.Exec();
	system("pause");
	return ret;
}

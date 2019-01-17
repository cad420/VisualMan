

#include "../src/application/glfwapplication.h"
#include "../src/application/guiapplication.h"
#include "../src/gui/transferfunctionwidget.h"

#include "../src/application/largevolumeraycasterapplication.h"


class TestGLFWApp :public ysl::app::GLFWApplication
{
public:
	TestGLFWApp(int argc, char *argv[], int w, int h);
protected:
	void InitializeOpenGLResources() override;
	void RenderLoop() override;

};

TestGLFWApp::TestGLFWApp(int argc, char* argv[], int w, int h):GLFWApplication(argc,argv,w,h)
{
}

void TestGLFWApp::InitializeOpenGLResources()
{
	//ysl::Log("InitializeOpenGLResources()");
	//GL_ERROR_REPORT
}

void TestGLFWApp::RenderLoop()
{
	//ysl::Log("Rendering in OpenGL.");
	//GL_ERROR_REPORT
}

class TestGuiApp:public ysl::app::ImGuiApplication
{
public:
	TestGuiApp(int argc, char * argv[], int w, int h);
protected:
	void InitializeOpenGLResources() override;
	void RenderLoop() override;
	void WindowResizeEvent(ResizeEvent* event) override;
};

TestGuiApp::TestGuiApp(int argc, char* argv[], int w, int h):
ImGuiApplication(argc,argv,w,h)
{

}

void TestGuiApp::InitializeOpenGLResources()
{
	const auto tf = std::make_shared<ysl::imgui::TransferFunctionWidget>("d:\\scidata\\tf1.tfi");
	AddWidget(tf);
}

void TestGuiApp::RenderLoop()
{
	//ysl::Log("TestGuiApp::RenderLoop()");
}

void TestGuiApp::WindowResizeEvent(ResizeEvent* event)
{
	std::cout << event->size() << std::endl;
}


int main(int argc,char ** argv)
{
	//return ysl::app::LargeVolumeRayCaster(argc, argv, 800, 600, "C:\\data\\s1_480_480_480_2_64.lvd").Exec();
	return TestGuiApp(argc, argv, 800, 600).Exec();
}




#include "../src/application/openglapplication.h"
#include "../src/utility/error.h"
#include "../src/opengl/openglutils.h"
#include "../src/application/guiapplication.h"
#include "../src/gui/transferfunctionwidget.h"


class TestOpenGLApp :public ysl::app::OpenGLApplication
{
public:
	TestOpenGLApp(int argc, char *argv[], int w, int h);
protected:
	void InitializeOpenGLResources() override;
	void RenderLoop() override;

};

TestOpenGLApp::TestOpenGLApp(int argc, char* argv[], int w, int h):OpenGLApplication(argc,argv,w,h)
{
}

void TestOpenGLApp::InitializeOpenGLResources()
{
	ysl::Log("InitializeOpenGLResources()");
	GL_ERROR_REPORT
}

void TestOpenGLApp::RenderLoop()
{
	ysl::Log("Rendering in OpenGL.");
	GL_ERROR_REPORT
}

class TestGuiApp:public ysl::app::GuiApplication
{
public:
	TestGuiApp(int argc, char * argv[], int w, int h);
protected:
	void InitializeOpenGLResources() override;
	void RenderLoop() override;
};

TestGuiApp::TestGuiApp(int argc, char* argv[], int w, int h):
GuiApplication(argc,argv,w,h)
{

}

void TestGuiApp::InitializeOpenGLResources()
{
	auto tf = std::make_shared<ysl::imgui::TransferFunctionWidget>("d:\\scidata\\tf1.tfi");
	AddWidget(tf);
}

void TestGuiApp::RenderLoop()
{
	ysl::Log("TestGuiApp::RenderLoop()");
}

int main(int argc,char ** argv)
{
	return TestGuiApp(argc, argv, 800,600).Exec();
}




#include "../src/application/openglapplication.h"
#include "../src/application/guiapplication.h"
#include "../src/gui/transferfunctionwidget.h"


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


class Base
{
public:
	Base()
	{
		app = this;
	}
	virtual void Fun() = 0;
	virtual ~Base() = default;
	static Base * app;
};

Base * Base::app = nullptr;

class DeriveA :public Base
{
public:
	DeriveA() = default;
	void Fun() override
	{
		std::cout << "DeriveA " << this << std::endl;

	}
	~DeriveA()=default;
};

class DeriveB :public DeriveA
{
public:
	DeriveB() = default;
	void Fun()override
	{
		std::cout << "DeriveB " << this << std::endl;

	}
	~DeriveB() = default;
};


int main(int argc,char ** argv)
{
	//return TestGLFWApp(argc, argv, 800, 600).Exec();
	return TestGuiApp(argc, argv, 800, 600).Exec();

	//Base * a = new DeriveB;
	//a->Fun();
	//

	//std::cout << "a: " << a << std::endl;
	//std::cout << "Base Pointer: " << static_cast<Base*>(a) << std::endl;
	//std::cout << "DeriveA Pointer: " << static_cast<DeriveA*>(a) << std::endl;
	//std::cout << "DeriveB Pointer: " << static_cast<DeriveB*>(a) << std::endl;


	//auto d1 = dynamic_cast<DeriveA*>(Base::app);
	//auto d2 = dynamic_cast<DeriveB*>(Base::app);

	//std::cout << "App pointer:" << Base::app << std::endl;
	//std::cout << "DeriveA Pointer from app: " << d1 << std::endl;
	//std::cout << "DeriveB Pointer from app: " << d2 << std::endl;

	//d1->Fun();
	//d2->Fun();
	//system("pause");
	//return 0;
}


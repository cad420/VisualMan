#include "../src/application/glfwapplication.h"



class NewGraphicApiTestApp:public ysl::app::GLFWApplication
{
public:
	NewGraphicApiTestApp(int argc, char ** argv) :GLFWApplication(argc, argv, 800, 600)
	{
		
	}
	void RenderLoop() override
	{
		
	}

	void InitializeOpenGLResources() override
	{

	}


};

int main(int argc,char ** argv)
{
	return NewGraphicApiTestApp(argc, argv).Exec();
}
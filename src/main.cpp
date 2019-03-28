


// MRE.cpp : Defines the entry point for the application.
// ImGui - standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)
// (GL3W is a helper library to access OpenGL functions since there is no standard header to access modern OpenGL functions easily. Alternatives are GLEW, Glad, etc.)

#include "application/largevolumeraycasterapplication.h"
#include "renderer/isosurfacerenderer.h"





int main(int argc, char ** argv)
{
	//return ysl::app::LargeVolumeRayCaster(argc, argv,
	//	800, 600, "C:\\data\\subregion_9000_10700_2_128.lvd").Exec();

	return ysl::app::LargeVolumeRayCaster(argc, argv,
		800, 600, "D:\\Desktop\\data\\test.lvd").Exec();


	//return ysl::app::LargeVolumeRayCaster(argc, argv,
	//	800, 600, "D:\\scidata\\abc\\s1_512_512_512_2_128.lvd").Exec();
	//return ysl::app::OITMeshRenderer(argc, argv, 800, 600).Exec();
}
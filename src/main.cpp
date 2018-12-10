// MRE.cpp : Defines the entry point for the application.
//


// ImGui - standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)
// (GL3W is a helper library to access OpenGL functions since there is no standard header to access modern OpenGL functions easily. Alternatives are GLEW, Glad, etc.)

#include "gui/imgui.h"
#include "gui/imgui_impl_glfw.h"
#include "gui/imgui_impl_opengl3.h"
#include <stdio.h>
#include <GL/gl3w.h>    // This example is using gl3w to access OpenGL functions. You may freely use any other OpenGL loader such as: glew, glad, glLoadGen, etc.
//#include <glew.h>
#include <GLFW/glfw3.h>
#include "../mathematics/geometry.h"


static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char**)
{
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui GLFW+OpenGL4 example", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync
	gl3wInit();

	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	// Setup style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them. 
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple. 
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'misc/fonts/README.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	bool show_my_first_imgui_window = true;

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();

		// Start the ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// 1. Show a simple window.
		// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
		//{
		//	static float f = 0.0f;
		//	static int counter = 0;
		//	ImGui::Text("Hello, world!");                           // Display some text (you can use a format string too)
		//	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
		//	ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		//	ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our windows open/close state
		//	ImGui::Checkbox("Another Window", &show_another_window);

		//	if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
		//		counter++;
		//	ImGui::SameLine();
		//	ImGui::Text("counter = %d", counter);

		//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//}


		if (show_my_first_imgui_window)
		{
			ImGui::Begin("My First ImGui Window", &show_my_first_imgui_window, ImGuiWindowFlags_MenuBar);

			static bool rayCastingMode = false;


			if (ImGui::BeginMenuBar()) {
				if (ImGui::BeginMenu("File")) {
					if (ImGui::MenuItem("Open..", "Ctrl+O")) {

					}
					if (ImGui::MenuItem("Save..", "Ctrl+S")) {

					}
					if (ImGui::MenuItem("Close", "Ctrl+W")) {
						show_my_first_imgui_window = false;
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Mode"))
				{
					if (ImGui::MenuItem("Large-Scale Ray-Casting"))
					{
						// list opengl information
						rayCastingMode = true;


					}
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			//if(rayCastingMode)
			//{
			GLint min,max;
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
			ImGui::Text("GL_MAX_TEXTURE_SIZE: %d", max);		// 32768

			glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &max);
			ImGui::Text("GL_MAX_3D_TEXTURE_SIZE:%d", max);
			glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max);
			ImGui::Text("GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS: %d", max);		//192

			glGetIntegerv(GL_MIN_PROGRAM_TEXEL_OFFSET, &min);
			ImGui::Text("GL_MIN_PROGRAM_TEXEL_OFFSET:%d", min);
			glGetIntegerv(GL_MAX_PROGRAM_TEXEL_OFFSET, &max);
			ImGui::Text("GL_MAX_PROGRAM_TEXEL_OFFSET:%d", max);

			glGetIntegerv(GL_MAX_FRAGMENT_IMAGE_UNIFORMS,&max);
			ImGui::Text("GL_MAX_FRAGMENT_IMAGE_UNIFORMS:%d", max);

			glGetIntegerv(GL_MAX_IMAGE_UNITS, &max);
			ImGui::Text("GL_MAX_IMAGE_UNITS:%d", max);

			// 
			//{
			//	GLuint texBuffer;
			//	glGenBuffers(1, &texBuffer);
			//	glBindBuffer(GL_TEXTURE_BUFFER, texBuffer);
			//	glBufferData(GL_TEXTURE_BUFFER, 100, nullptr, GL_STATIC_DRAW);

			//	GLuint texture;
			//	glGenTextures(1, &texture);
			//	glBindTexture(GL_TEXTURE_BUFFER, texture);
			//	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, texBuffer);
			//}
			ImGui::End();
		}


		// 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name your windows.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}

		// 3. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
		if (show_demo_window)
		{
			ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
			ImGui::ShowDemoWindow(&show_demo_window);
		}



		ImGui::EndFrame();
		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
	}

	// Test Code


	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

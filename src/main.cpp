// MRE.cpp : Defines the entry point for the application.
// ImGui - standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)
// (GL3W is a helper library to access OpenGL functions since there is no standard header to access modern OpenGL functions easily. Alternatives are GLEW, Glad, etc.)

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"


#include <stdio.h>
#include <GL/gl3w.h>    // This example is using gl3w to access OpenGL functions. You may freely use any other OpenGL loader such as: glew, glad, glLoadGen, etc.

#include <GLFW/glfw3.h>

//#include "../mathematics/geometry.h"

#include "gui/TinyConsole.h"
#include "gui/widget.h"

#include "utility/cmdline.h"
#include "volume/volume_utils.h"
#include "volume/volume.h"

#include "cameras/camera.h"

#include "opengl/shader.h"


class Event
{
};


/*
 * The reason why make the members public and supports corresponding
 * access methods as the same time is that these class should be redesigned later conform to OO
 */

class MouseEvent :public Event
{
public:
	enum { LeftButton = 1, RightButton = 2 };

	ysl::Point2i m_pos;
	int m_buttons;
	MouseEvent(const ysl::Point2i & pos, int button) :m_pos(pos), m_buttons(button) {}
	ysl::Point2i pos()const { return m_pos; }
	int buttons()const { return m_buttons; }
};

class KeyboardEvent :public Event
{
public:
	int m_key;
	KeyboardEvent(int key) :m_key(key) {}
	int key()const { return m_key; }
};

class ResizeEvent:public Event
{
public:
	ysl::Vector2i m_size;
	ResizeEvent(const ysl::Vector2i & size):m_size(size){}
	const ysl::Vector2i & size()const { return m_size;}
};



/**************************************************/
FocusCamera g_camera{ ysl::Point3f{0.f,0.f,10.f} };
ysl::ShaderProgram g_shaderProgram;
ysl::Point2i g_lastMousePos;

void renderingWindowResize(ResizeEvent *event)
{

};


void mousePressedEvent(MouseEvent * event)
{
	g_lastMousePos = event->pos();
}

void mouseMoveEvent(MouseEvent * event)
{
	const auto & p = event->pos();
	// Update Camera
	float dx = p.x - g_lastMousePos.x;
	float dy = g_lastMousePos.y - p.y;
	if ((event->buttons() & MouseEvent::LeftButton) && (event->buttons() & MouseEvent::RightButton))
	{
		const auto directionEx = g_camera.up()*dy + dx * g_camera.right();
		g_camera.movement(directionEx, 0.002);
	}
	else if (event->buttons() & MouseEvent::LeftButton)
	{
		g_camera.rotation(dx, dy);
	}
	else if (event->buttons() == MouseEvent::RightButton)
	{
		const auto directionEx = g_camera.front()*dy;
		g_camera.movement(directionEx, 0.01);
	}
	std::cout << g_camera.view();
	g_lastMousePos = p;
}

void mouseReleaseEvent(MouseEvent * event)
{

}

void keyboardPressEvent(KeyboardEvent)
{

}

void renderLoop()
{
	g_shaderProgram.bind();
	g_shaderProgram.unbind();
}

static const char trivialVertexShader[] = "#version 330\n"
"uniform mat4 modelViewMat;\n"
"uniform mat4 projMat;\n"
"layout(location = 0) in vec3 vertex;\n"
"void main()\n"
"{\n"
"	gl_Position = projMat * modelViewMat*vec4(vertex, 1.0);\n"
"}\n";

static const char trivialFragShader[] = "#version 330\n"
"out vec4 fgColor;\n"
"uniform vec4 color;\n"
"void main()\n"
"{\n"
"	fgColor = color;\n"
"}\n";


/**************************************************/

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int argc, char** argv)
{
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	GLFWwindow* window = glfwCreateWindow(1280, 720, "Mixed Render Engine", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync
	gl3wInit();

	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO & io = ImGui::GetIO();
	(void)io;

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
	//bool show_my_first_imgui_window = true;


	// 'Global' state

	///![1] console 
	TinyConsole app;
		// Config Commands
		std::shared_ptr<LargeVolumeCache> cache;

		app.ConfigCommand("load", [&cache](const char * cmd)
		{
			cmdline::parser p;
			p.add<std::string>("file", 'f', "load large volume data", false);
			p.parse(cmd);
			const auto fileName = p.get<std::string>("file");

			if (!fileName.empty())
				cache = std::make_shared<LargeVolumeCache>(fileName);
		});
		auto showGLInformation = false;
		app.ConfigCommand("glinfo", [&showGLInformation](const char * cmd)
		{
			showGLInformation = true;
		});

	///![2] window size
	ysl::Vector2i windowSize;

	g_shaderProgram.create();
	g_shaderProgram.addShaderFromSourceCode(trivialVertexShader, ysl::ShaderProgram::ShaderType::Vertex);
	g_shaderProgram.addShaderFromSourceCode(trivialFragShader, ysl::ShaderProgram::ShaderType::Fragment);
	g_shaderProgram.link();


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


		app.Draw("Welcome Mixed Render Engine", nullptr);


		// 1. Show a simple window.
		//// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
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

		//ImGui::Begin("IO Demo", nullptr);
		//if(ImGui::IsMousePosValid())
		//{
		//	ImGui::Text("Mouse Pos:[%g, %g]", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
		//}
		//if (ImGui::IsMouseClicked(0))
		//	ImGui::Text("Mouse Click:%d", 0);

		//ImGui::End();
		//ImGui::ShowDemoWindow();

		ImGui::Text("[%g, %g]", io.DisplaySize.x, io.DisplaySize.y);

		if (showGLInformation)
		{
			ShowGLInformation();
		}



		// Event handle
		if (ImGui::IsMousePosValid())
		{
			MouseEvent pressEvent({ 0,0 }, 0);
			for (auto i = 0; i < IM_ARRAYSIZE(io.MouseDown); ++i)
			{
				// Only support two buttons now

				if (ImGui::IsMouseClicked(i))
				{
					ysl::Point2i pos{ int(io.MousePos.x),int(io.MousePos.y) };
					switch (i)
					{
					case 0:pressEvent.m_buttons |= MouseEvent::LeftButton; pressEvent.m_pos = pos; break;
					case 1:pressEvent.m_buttons |= MouseEvent::RightButton; pressEvent.m_pos = pos; break;
					default:break;
					}
				}
			}
			if (pressEvent.m_buttons != 0)
			{
				mousePressedEvent(&pressEvent);
			}
		}

		// Move Event
		MouseEvent moveEvent({ 0,0 }, 0);
		for (auto i = 0; i < IM_ARRAYSIZE(io.MouseDown); ++i)
		{
			if (io.MouseDownDuration[i] > 0.0f)
			{
				
				if (io.MouseDelta.x != 0 || io.MouseDelta.y != 0)
				{
					ysl::Point2i pos{ int(io.MousePos.x),int(io.MousePos.y) };
					switch (i)
					{
					case 0:moveEvent.m_buttons |= MouseEvent::LeftButton; moveEvent.m_pos = pos; break;
					case 1:moveEvent.m_buttons |= MouseEvent::RightButton; moveEvent.m_pos = pos; break;
					default:break;
					}
				}
			}
		}
		if (moveEvent.m_buttons != 0)
		{
			mouseMoveEvent(&moveEvent);
		}


		// Release Event

		MouseEvent releaseEvent({ 0,0 }, 0);
		for (auto i = 0; i < IM_ARRAYSIZE(io.MouseDown); ++i)
		{
			if (ImGui::IsMouseReleased(i))
			{
				ysl::Point2i pos{ int(io.MousePos.x),int(io.MousePos.y) };
				switch (i)
				{
				case 0:releaseEvent.m_buttons |= MouseEvent::LeftButton; releaseEvent.m_pos = pos; break;
				case 1:releaseEvent.m_buttons |= MouseEvent::RightButton; releaseEvent.m_pos = pos; break;
				default:break;
				}
			}
		}

		if (releaseEvent.m_buttons != 0)
			mouseReleaseEvent(&releaseEvent);


		// Rendering window resize Event
		if(io.DisplaySize.x != windowSize.x || io.DisplaySize.y != windowSize.y)
		{
			windowSize = {int(io.DisplaySize.x),int(io.DisplaySize.y)};
			ResizeEvent event(windowSize);
			renderingWindowResize(&event);
		}


		renderLoop();


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

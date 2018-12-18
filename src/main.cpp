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
#include "opengl/openglbuffer.h"
#include "openglvertexarrayobject.h"


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

class ResizeEvent :public Event
{
public:
	ysl::Vector2i m_size;
	ResizeEvent(const ysl::Vector2i & size) :m_size(size) {}
	const ysl::Vector2i & size()const { return m_size; }
};


/**************************************************/
FocusCamera g_camera{ ysl::Point3f{0.f,0.f,10.f} };
ysl::Transform g_projMatrix;
ysl::ShaderProgram g_shaderProgram;
ysl::Point2i g_lastMousePos;
std::shared_ptr<LargeVolumeCache> cache;

std::unique_ptr<char[]> g_rawData;
std::vector<ysl::RGBASpectrum> m_tfData{256};
ysl::TransferFunction m_tfObject;
unsigned int g_volumeTexture;
unsigned int g_tfTexture;
unsigned int VBO, VAO;

OpenGLBuffer g_vbo(OpenGLBuffer::BufferType::VertexArrayBuffer);
OpenGLVertexArrayObject g_vao;


void renderingWindowResize(ResizeEvent *event)
{
	float aspect = static_cast<float>(event->size().x) / static_cast<float>(event->size().y);
	g_projMatrix.SetPerspective(45.0f, aspect, 0.01, 100);
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
	//std::cout << g_camera.view();
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
	//glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);

	g_shaderProgram.bind();
	g_shaderProgram.setUniformValue("modelViewMat", g_camera.view().Matrix());
	g_shaderProgram.setUniformValue("projMat", g_projMatrix.Matrix());
	auto color = ysl::RGBSpectrum{ 0.3 };
	g_shaderProgram.setUniformValue("color", color);

	//glBindVertexArray(VAO);
	g_vao.bind();
	GL_ERROR_REPORT
	glDrawArrays(GL_TRIANGLES, 0, 3);
	GL_ERROR_REPORT
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
"uniform vec3 color;\n"
"void main()\n"
"{\n"
"	fgColor = vec4(color,1.0);\n"
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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
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


	app.ConfigCommand("load_data", [](const char * cmd)
	{
		//cmdline::parser p;
		//p.add<std::string>("file", 'f', "load large volume data", false);
		//p.parse(cmd);
		//const auto fileName = p.get<std::string>("file");

		//if (!fileName.empty())
		//{
		//	cache = std::make_shared<LargeVolumeCache>(fileName);
		//	if (cache != nullptr) {
		//		const std::size_t x = cache->reader().width();
		//		const std::size_t y = cache->reader().height();
		//		const std::size_t z = cache->reader().depth();
		//		std::cout << x << " " << y << " " << z << std::endl;
		//	}
		//}

	});

	app.ConfigCommand("load_tf",[](const char * cmd)
	{
		cmdline::parser p;
		p.add<std::string>("tf", 'f', "load transfer function", false);
		p.parse(cmd);
		const auto fileName = p.get<std::string>("tf");

		std::cout << "fileName:" << fileName;

		m_tfObject.read(fileName);
		if(!m_tfObject.valid())
		{
			std::cout << "Reading failed\n";
			return;
		}
			
		m_tfObject.FetchData(m_tfData.data(), 256);

		for(int i = 0 ; i < 256;i++)
		{
			std::cout << m_tfData[i];
		}

	});

	auto showGLInformation = false;
	app.ConfigCommand("glinfo", [&showGLInformation](const char * cmd)
	{
		showGLInformation = true;
	});

	///![2] window size
	ysl::Vector2i windowSize;

	// gl resources

	// shader
	g_shaderProgram.create();
	//g_shaderProgram.addShaderFromSourceCode(trivialVertexShader, ysl::ShaderProgram::ShaderType::Vertex);
	//g_shaderProgram.addShaderFromSourceCode(trivialFragShader, ysl::ShaderProgram::ShaderType::Fragment);
	g_shaderProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\blockraycasting_f.glsl", ysl::ShaderProgram::ShaderType::Vertex);
	g_shaderProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\blockraycasting_f.glsl", ysl::ShaderProgram::ShaderType::Fragment);
	g_shaderProgram.link();

	static float vertices[] = 
	{ -0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f };

	// vao vbo
	g_vao.create();
	g_vao.bind();
	g_vbo.create();
	g_vbo.bind();
	g_vbo.allocate(vertices, sizeof(vertices));

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));

	// texture

	std::string dataFileName;
	std::cin >> dataFileName;

	std::size_t width, height, depth;
	std::cin >> width >> height >> depth;
	std::size_t total = width * height*depth;
	std::ifstream rawData(dataFileName);
	if(!rawData.is_open())
	{
		std::cout << "Can not open raw data\n";
		return 0;
	}

	

	g_rawData.reset(new char[total]);
	if(g_rawData == nullptr)
	{
		std::cout << "Bad alloc\n";
		return 0;
	}

	rawData.read(g_rawData.get(), total * sizeof(char));

	glGenTextures(1, &g_volumeTexture);
	glBindTexture(GL_TEXTURE_3D, g_volumeTexture);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, width, height, depth, 0, GL_RED, GL_UNSIGNED_BYTE,g_rawData.get());


	glGenTextures(1, &g_tfTexture);
	glBindTexture(GL_TEXTURE_1D, g_tfTexture);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	//GL_ERROR_REPORT




	//cache = std::make_shared<LargeVolumeCache>(dataFileName);

	//if(cache)
	//{
	//	const std::size_t x = cache->reader().width();
	//	const std::size_t y = cache->reader().height();
	//	const std::size_t z = cache->reader().depth();
	//	std::cout << x << " " << y << " " << z << std::endl;
	//}


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



		//ImGui::End();
		//ImGui::ShowDemoWindow();


		if (showGLInformation)ShowGLInformation(&showGLInformation);

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
		if (io.DisplaySize.x != windowSize.x || io.DisplaySize.y != windowSize.y)
		{
			windowSize = { int(io.DisplaySize.x),int(io.DisplaySize.y) };
			ResizeEvent event(windowSize);
			renderingWindowResize(&event);
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

		// User's render is here
		renderLoop();

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

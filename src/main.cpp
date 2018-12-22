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
#include "opengl/openglvertexarrayobject.h"
#include "openglutils.h"
#include "volume/virtualvolumehierachy.h"
#include "GL/glcorearb.h"


//const static int g_proxyGeometryVertexIndices[] = { 1, 3, 7, 5, 0, 4, 6, 2, 2, 6, 7, 3, 0, 1, 5, 4, 4, 5, 7, 6, 0, 2, 3, 1 };
const static int g_proxyGeometryVertexIndices[] = { 1,3,7,1,7,5,0,4,6,0,6,2,2,6,7,2,7,3,0,1,5,0,5,4,4,5,7,4,7,6,0,2,3,0,3,1 };
static const float xCoord = 1.0, yCoord = 1.0, zCoord = 1.0;
static float g_proxyGeometryVertices[] = {
	0,0,0,
	xCoord, 0.0, 0.0 ,
	0 , yCoord , 0 ,
	xCoord , yCoord , 0 ,
	0 , 0 , zCoord  ,
	xCoord , 0 , zCoord ,
	0 , yCoord , zCoord,
	xCoord , yCoord , zCoord ,
		-0.5,0 - 0.5,0 - 0.5,
	xCoord - 0.5, 0.0 - 0.5, 0.0 - 0.5 ,
	0 - 0.5 , yCoord - 0.5, 0 - 0.5,
	xCoord - 0.5, yCoord - 0.5 , 0 - 0.5,
	0 - 0.5, 0 - 0.5 , zCoord - 0.5 ,
	xCoord - 0.5 , 0 - 0.5, zCoord - 0.5,
	0 - 0.5 , yCoord - 0.5, zCoord - 0.5,
	xCoord - 0.5, yCoord - 0.5, zCoord - 0.5,
};

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




/**************************************************/

namespace
{

	//LargeVolumeCache g_volumeCache;
	FocusCamera g_camera{ ysl::Point3f{0.f,0.f,10.f} };
	ysl::Transform g_projMatrix;
	ysl::Transform g_orthoMatrix;
	ysl::ShaderProgram g_rayCastingShaderProgram;
	ysl::ShaderProgram g_positionShaderProgram;
	ysl::Point2i g_lastMousePos;
	std::shared_ptr<LargeVolumeCache> cache;

	std::unique_ptr<char[]> g_rawData;
	std::vector<ysl::RGBASpectrum> m_tfData{ 256 };
	ysl::TransferFunction m_tfObject;

	ysl::Vector3f g_lightDirection;

	float step = 0.01;
	float ka = 1.0;
	float ks = 1.0;
	float kd = 1.0;
	float shininess = 50.0f;
	float tmin = 2;
	float alpha = 0.4;
	float beta = 0.5;
	float radius = 0.5;
	float v[3];


	constexpr int pageTableBlockEntry = 16;
	const std::size_t missedBlockCapacity = 10000 + sizeof(unsigned int) + 2 * sizeof(unsigned int);

	unsigned int g_volumeTexture;
	unsigned int g_tfTexture;
	unsigned int g_framebufferObject;
	unsigned int g_entryPointTextureId;
	unsigned int g_depthTextureId;
	unsigned int g_exitPointTextureId;

	OpenGLBuffer g_proxyEBO(OpenGLBuffer::BufferType::ElementArrayBuffer);
	OpenGLBuffer g_proxyVBO(OpenGLBuffer::BufferType::VertexArrayBuffer);
	OpenGLVertexArrayObject g_proxyVAO;

	OpenGLBuffer g_rayCastingVBO(OpenGLBuffer::BufferType::VertexArrayBuffer);
	OpenGLVertexArrayObject g_rayCastingVAO;

	unsigned int g_pageDirTexture;
	unsigned int g_pageTableTexture;
	unsigned int g_cacheTexture;
	unsigned int g_existBufferId;
	unsigned int g_missedBlockBufferId;


	std::unique_ptr<VolumeVirtualMemoryHierachyGenerator<pageTableBlockEntry, pageTableBlockEntry, pageTableBlockEntry>> largeVolumeData;

	int pageDirX;
	int pageDirY;
	int pageDirZ;

	int pageTableX;
	int pageTableY;
	int pageTableZ;

	int cacheWidth;
	int cacheHeight;
	int cacheDepth;
	int repeat;
	int blockDataSize;
	int originalDataWidth;
	int originalDataHeight;
	int originalDataDepth;

	int xBlockCount;
	int yBlockCount;
	int zBlockCount;

}


void checkFrambufferStatus();

void renderingWindowResize(ResizeEvent *event)
{
	const auto x = event->size().x;
	const auto y = event->size().y;
	g_orthoMatrix.SetOrtho(0, x, 0, y, -10, 100);

	//
	const auto aspect = static_cast<float>(x) / static_cast<float>(y);
	g_projMatrix.SetPerspective(45.0f, aspect, 0.01, 100);

	// Update frambuffer size


	glBindTexture(GL_TEXTURE_RECTANGLE, g_entryPointTextureId);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA32F, x, y, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_RECTANGLE, g_exitPointTextureId);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA32F, x, y, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_RECTANGLE, g_depthTextureId);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH_COMPONENT, x, y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	// update raycastingtexture
	float windowSize[] =
	{
		0,0,x,0,0,y,0,y,x,0,x,y
	};

	g_rayCastingVBO.bind();
	g_rayCastingVBO.allocate(windowSize, sizeof(windowSize));

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

void checkFrambufferStatus()
{
	glBindFramebuffer(GL_FRAMEBUFFER, g_framebufferObject);


	GLenum status;
	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (status) {
	case GL_FRAMEBUFFER_COMPLETE: // Everything's OK

		break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		std::cout << "glift::CheckFramebufferStatus() ERROR:\n\t"
			<< "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		std::cout << "glift::CheckFramebufferStatus() ERROR:\n\t"
			<< "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n";
		break;
	case GL_FRAMEBUFFER_UNDEFINED:
		std::cout << "glift::CheckFramebufferStatus() ERROR:\n\t"
			<< "GL_FRAMEBUFFER_UNDEFINED\n";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		std::cout << "glift::CheckFramebufferStatus() ERROR:\n\t"
			<< "GL_FRAMEBUFFER_INCOMPLETE_FORMATS\n";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		std::cout << "glift::CheckFramebufferStatus() ERROR:\n\t"
			<< "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER\n";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		std::cout << "glift::CheckFramebufferStatus() ERROR:\n\t"
			<< "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER\n";
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		std::cout << "glift::CheckFramebufferStatus() ERROR:\n\t"
			<< "GL_FRAMEBUFFER_UNSUPPORTED\n";
		break;
	default:
		std::cout << "glift::CheckFramebufferStatus() ERROR:\n\t"
			<< "Unknown ERROR\n";

	}

}

void initBlockExistsHash()
{
	// exist buffer id
	glGenBuffers(1, &g_existBufferId);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, g_existBufferId);
	auto ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	const std::size_t totalBlockCount = std::size_t(xBlockCount) * std::size_t(yBlockCount)*std::size_t(zBlockCount) * sizeof(int);
	memset(ptr, 0, totalBlockCount);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

}

void initMissedBlockVector()
{

	// missing block id
	glGenBuffers(1, &g_missedBlockBufferId);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, g_missedBlockBufferId);
	auto ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	memset(ptr, 0, missedBlockCapacity);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

bool isCacheMissed()
{
	return false;
}

void processCacheMiss()
{

}




void renderLoop()
{
	//glClear(GL_COLOR_BUFFER_BIT);
	GL_ERROR_REPORT;
	glEnable(GL_DEPTH_TEST);
	GL_ERROR_REPORT;
	g_positionShaderProgram.bind();
	g_positionShaderProgram.setUniformValue("projMatrix", g_projMatrix.Matrix());
	g_positionShaderProgram.setUniformValue("worldMatrix", ysl::Transform{}.Matrix());
	g_positionShaderProgram.setUniformValue("viewMatrix", g_camera.view().Matrix());
	g_proxyVAO.bind();

	glBindFramebuffer(GL_FRAMEBUFFER, g_framebufferObject);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	// Draw front
	glDepthFunc(GL_LESS);

	GL_ERROR_REPORT;
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	GL_ERROR_REPORT;
	// Draw Back
	glDrawBuffer(GL_COLOR_ATTACHMENT1);
	glClear(GL_COLOR_BUFFER_BIT);
	glDepthFunc(GL_GREATER);

	GL_ERROR_REPORT;
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	GL_ERROR_REPORT;

	glDepthFunc(GL_LESS);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GL_ERROR_REPORT;

	glEnable(GL_BLEND);
	GL_ERROR_REPORT;
	glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
	GL_ERROR_REPORT;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GL_ERROR_REPORT;
	glEnable(GL_TEXTURE_RECTANGLE);
	GL_ERROR_REPORT;

	g_rayCastingShaderProgram.bind();
	g_rayCastingShaderProgram.setUniformValue("viewMatrix", g_camera.view().Matrix());
	g_rayCastingShaderProgram.setUniformValue("orthoMatrix", g_orthoMatrix.Matrix());
	g_rayCastingShaderProgram.setUniformSampler("texVolume", TextureUnit3, Texture3D, g_volumeTexture);
	g_rayCastingShaderProgram.setUniformSampler("texStartPos", TextureUnit0, Texture2DRect, g_entryPointTextureId);
	g_rayCastingShaderProgram.setUniformSampler("texEndPos", TextureUnit1, Texture2DRect, g_exitPointTextureId);
	g_rayCastingShaderProgram.setUniformSampler("texTransfunc", TextureUnit2, Texture1D, g_tfTexture);
	g_rayCastingShaderProgram.setUniformValue("eye", g_camera.position());
	g_rayCastingShaderProgram.setUniformValue("forward",g_camera.front());
	g_rayCastingShaderProgram.setUniformValue("tmin", tmin);
	g_rayCastingShaderProgram.setUniformValue("alpha", alpha);
	g_rayCastingShaderProgram.setUniformValue("beta", beta);
	g_rayCastingShaderProgram.setUniformValue("radius", radius);
	g_rayCastingShaderProgram.setUniformValue("axis", ysl::Vector3f{v[0],v[1],v[2]});

	g_rayCastingShaderProgram.setUniformValue("step", step);
	g_rayCastingShaderProgram.setUniformValue("ka", ka);
	g_rayCastingShaderProgram.setUniformValue("ks", ks);
	g_rayCastingShaderProgram.setUniformValue("kd", kd);
	g_rayCastingShaderProgram.setUniformValue("shininess", shininess);
	g_rayCastingShaderProgram.setUniformValue("lightdir", g_lightDirection);

	auto halfWay = g_lightDirection - g_camera.front();
	if (halfWay.Length() > 1e-10) halfWay.Normalize();
	g_rayCastingShaderProgram.setUniformValue("halfway", halfWay);

	g_rayCastingVAO.bind();

	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisable(GL_DEPTH_TEST);

	if (isCacheMissed())
	{
		processCacheMiss();
	}

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

	int initialWidth = 1280, initialHeight = 720;

	GLFWwindow* window = glfwCreateWindow(initialWidth, initialHeight, "Mixed Render Engine", NULL, NULL);
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

	app.ConfigCommand("load_tf", [](const char * cmd)
		{
			cmdline::parser p;
			p.add<std::string>("tf", 'f', "load transfer function", false);
			p.parse(cmd);
			const auto fileName = p.get<std::string>("tf");

			std::cout << "fileName:" << fileName;

			ysl::TransferFunction m_tfObject;
			m_tfObject.read(fileName);
			if (!m_tfObject.valid())
			{
				std::cout << "Reading failed\n";
				return;
			}

			m_tfObject.FetchData(m_tfData.data(), 256);

			glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, 256, 0, GL_RGBA, GL_FLOAT, m_tfData.data());

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
	g_rayCastingShaderProgram.create();
	g_rayCastingShaderProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\raycasting_v.glsl", ysl::ShaderProgram::ShaderType::Vertex);
	g_rayCastingShaderProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\raycasting_f.glsl", ysl::ShaderProgram::ShaderType::Fragment);
	g_rayCastingShaderProgram.link();

	g_positionShaderProgram.create();
	g_positionShaderProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\position_v.glsl", ysl::ShaderProgram::ShaderType::Vertex);
	g_positionShaderProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\position_f.glsl", ysl::ShaderProgram::ShaderType::Fragment);
	g_positionShaderProgram.link();

	static float g_vertices[] =
	{
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f
	};

	// vao vbo
	GL_ERROR_REPORT
		g_proxyVAO.create();
	g_proxyVAO.bind();

	g_proxyVBO.create();
	g_proxyVBO.bind();
	g_proxyVBO.allocate(g_proxyGeometryVertices, sizeof(g_proxyGeometryVertices));

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(sizeof(g_proxyGeometryVertices) / 2));
	GL_ERROR_REPORT
		glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));
	GL_ERROR_REPORT

		g_proxyEBO.create();
	g_proxyEBO.bind();
	g_proxyEBO.allocate(g_proxyGeometryVertexIndices, sizeof(g_proxyGeometryVertexIndices));
	GL_ERROR_REPORT

		g_proxyVAO.unbind();


	g_rayCastingVAO.create();
	g_rayCastingVAO.bind();
	g_rayCastingVBO.create();
	g_rayCastingVBO.bind();
	g_rayCastingVBO.allocate(nullptr, 6 * sizeof(ysl::Point2f));

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<void*>(0));

	g_rayCastingVAO.unbind();

	// texture

	std::string dataFileName;
	//std::cin >> dataFileName;
	dataFileName = "D:\\scidata\\abc\\s1_302_302_302.raw";

	std::size_t width, height, depth;
	width = 302; height = 302, depth = 302;
	//std::cin >> width >> height >> depth;
	const std::size_t total = width * height * depth;
	std::ifstream rawData(dataFileName);
	if (!rawData.is_open())
	{
		std::cout << "Can not open raw data\n";
		return 0;
	}


	g_rawData.reset(new char[total]);
	if (g_rawData == nullptr)
	{
		std::cout << "Bad alloc\n";
		return 0;
	}
	rawData.read(g_rawData.get(), total * sizeof(char));

	std::string lvdFileName = "D:\\scidata\\abc\\s1_512_512_512.lvd";


	//VolumeVirtualMemoryHierachyGenerator<pageTableBlockEntry, pageTableBlockEntry, pageTableBlockEntry> largeVolumeData(lvdFileName);
	largeVolumeData.reset(new VolumeVirtualMemoryHierachyGenerator<pageTableBlockEntry, pageTableBlockEntry, pageTableBlockEntry>(lvdFileName));


	//largeVolumeData.m_pageDir->Width();

	pageDirX = largeVolumeData->m_pageDir->Width();
	pageDirY = largeVolumeData->m_pageDir->Height();
	pageDirZ = largeVolumeData->m_pageDir->Depth();

	pageTableX = largeVolumeData->m_pageTable->Width();
	pageTableY = largeVolumeData->m_pageTable->Height();
	pageTableZ = largeVolumeData->m_pageTable->Depth();

	cacheWidth = largeVolumeData->cacheWidth();
	cacheHeight = largeVolumeData->cacheHeight();
	cacheDepth = largeVolumeData->cacheDepth();
	repeat = largeVolumeData->repeat();
	blockDataSize = largeVolumeData->blockSize();
	originalDataWidth = largeVolumeData->originalWidth();
	originalDataHeight = largeVolumeData->originalHeight();
	originalDataDepth = largeVolumeData->originalDepth();

	xBlockCount = largeVolumeData->xBlockCount();
	yBlockCount = largeVolumeData->yBlockCount();
	zBlockCount = largeVolumeData->zBlockCount();

	const std::size_t totalBlockCountBytes = std::size_t(xBlockCount) * std::size_t(yBlockCount)*std::size_t(zBlockCount)*sizeof(int);

	std::cout << "pageDirX:" << pageDirX << std::endl;
	std::cout << "pageDirY:" << pageDirY << std::endl;
	std::cout << "pageDirZ:" << pageDirZ << std::endl;

	std::cout << "pageTableX:" << pageTableX << std::endl;
	std::cout << "pageTableY:" << pageTableY << std::endl;
	std::cout << "pageTableZ:" << pageTableZ << std::endl;

	std::cout << "cacheWidth:" << cacheWidth << std::endl;
	std::cout << "cacheHeight:" << cacheHeight << std::endl;
	std::cout << "cacheDepth:" << cacheDepth << std::endl;

	std::cout << "repeat:" << repeat << std::endl;
	std::cout << "data size:" << largeVolumeData->width() << std::endl;
	std::cout << "original data x:" << originalDataWidth << std::endl;
	std::cout << "original data y:" << originalDataHeight << std::endl;
	std::cout << "original data z:" << originalDataDepth << std::endl;


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Volume Texture
	glGenTextures(1, &g_volumeTexture);
	glBindTexture(GL_TEXTURE_3D, g_volumeTexture);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, width, height, depth, 0, GL_RED, GL_UNSIGNED_BYTE, g_rawData.get());
	GL_ERROR_REPORT;

	// page dir
	glGenTextures(1, &g_pageDirTexture);
	GL_ERROR_REPORT;
	glBindTexture(GL_TEXTURE_3D, g_pageDirTexture);
	GL_ERROR_REPORT;
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32UI, pageDirX, pageDirY, pageDirZ, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, largeVolumeData->m_pageDir->Data());
	GL_ERROR_REPORT;
	glBindTexture(GL_TEXTURE_3D, 0);
	GL_ERROR_REPORT;
	glBindImageTexture(0, g_pageDirTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32UI);			// layout(binding = 0)
	GL_ERROR_REPORT;

	// page table
	glGenTextures(1, &g_pageTableTexture);
	GL_ERROR_REPORT;
	glBindTexture(GL_TEXTURE_3D, g_pageTableTexture);
	GL_ERROR_REPORT;
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32UI, pageTableX, pageTableY, pageTableZ, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, largeVolumeData->m_pageTable->Data());
	GL_ERROR_REPORT;
	glBindTexture(GL_TEXTURE_3D, 0);
	glBindImageTexture(1, g_pageTableTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32UI);		// layout(binding = 1)
	GL_ERROR_REPORT;

	// data cache
	glGenTextures(1, &g_cacheTexture);
	glBindTexture(GL_TEXTURE_3D, g_cacheTexture);
	GL_ERROR_REPORT;
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, cacheWidth, cacheHeight, cacheDepth, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	GL_ERROR_REPORT;


	/// TODO::
	// exist buffer id
	glGenBuffers(1, &g_existBufferId);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, g_existBufferId);
	glBufferData(GL_SHADER_STORAGE_BUFFER, totalBlockCountBytes, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, g_existBufferId);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, 0);

	// missing block id
	glGenBuffers(1, &g_missedBlockBufferId);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, g_missedBlockBufferId);
	glBufferData(GL_SHADER_STORAGE_BUFFER, missedBlockCapacity, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, g_missedBlockBufferId);
	//auto ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);



	// shader program
	g_rayCastingShaderProgram.bind();
	g_rayCastingShaderProgram.setUniformSampler("cacheVolume", TextureUnit4, Texture3D, g_cacheTexture);
	g_rayCastingShaderProgram.setUniformValue("totalPageDirSize", ysl::Vector3i{ pageDirX,pageDirY,pageDirZ });
	g_rayCastingShaderProgram.setUniformValue("totalPageTableSize", ysl::Vector3i{ pageTableX,pageTableY,pageTableZ });
	g_rayCastingShaderProgram.setUniformValue("blockDataSize", ysl::Vector3i{ blockDataSize - 2 * repeat,blockDataSize - 2 * repeat,blockDataSize - 2 * repeat });
	g_rayCastingShaderProgram.setUniformValue("volumeDataSize", ysl::Vector3i{ originalDataWidth,originalDataDepth,originalDataDepth });
	g_rayCastingShaderProgram.setUniformValue("pageTableBlockEntrySize", ysl::Vector3i{ pageTableBlockEntry ,pageTableBlockEntry ,pageTableBlockEntry });
	g_rayCastingShaderProgram.setUniformValue("repeatSize", ysl::Vector3i{ repeat,repeat,repeat });
	g_rayCastingShaderProgram.unbind();


	// Transfer function texture
	glGenTextures(1, &g_tfTexture);
	glBindTexture(GL_TEXTURE_1D, g_tfTexture);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	GL_ERROR_REPORT;

	glGenTextures(1, &g_entryPointTextureId);
	glBindTexture(GL_TEXTURE_RECTANGLE, g_entryPointTextureId);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA32F, initialWidth, initialHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	GL_ERROR_REPORT;
	glGenTextures(1, &g_exitPointTextureId);
	glBindTexture(GL_TEXTURE_RECTANGLE, g_exitPointTextureId);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA32F, initialWidth, initialHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	GL_ERROR_REPORT;
	glGenTextures(1, &g_depthTextureId);
	glBindTexture(GL_TEXTURE_RECTANGLE, g_depthTextureId);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH_COMPONENT, initialWidth, initialHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);


	GL_ERROR_REPORT;
	glGenFramebuffers(1, &g_framebufferObject);
	GL_ERROR_REPORT;
	glBindFramebuffer(GL_FRAMEBUFFER, g_framebufferObject);
	GL_ERROR_REPORT;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, g_entryPointTextureId, 0);
	GL_ERROR_REPORT;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE, g_exitPointTextureId, 0);
	GL_ERROR_REPORT;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_RECTANGLE, g_depthTextureId, 0);
	GL_ERROR_REPORT;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GL_ERROR_REPORT;
	checkFrambufferStatus();



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
		static bool lock = true;
		ImGui::Begin("Control Panel");
		ImGui::SliderFloat("step", &step, 0.001, 1.0);
		ImGui::SliderFloat("plane", &tmin, 0.01, 10);
		ImGui::SliderFloat("alpha", &alpha, 0.00, 10);
		ImGui::SliderFloat("beta", &beta, 0.0, 100);
		ImGui::SliderFloat("radius", &radius, 0.05, 100);
		ImGui::SliderFloat("ka", &ka, 0.0f, 1.0f);
		ImGui::SliderFloat("kd", &kd, 0.0f, 1.0f);
		ImGui::SliderFloat("ks", &ks, 0.0f, 1.0f);
		ImGui::SliderFloat("shininess", &shininess, 0.0f, 50.f);
		ImGui::SliderFloat3("axis:", v, 0, 1);
		if(ImGui::Button("lock"))
		{
			lock = !lock;
		}
		ImGui::End();


		if (showGLInformation)
			ShowGLInformation(&showGLInformation);

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
			if (io.MouseDownDuration[i] > 0.0f && !ImGui::IsMouseHoveringAnyWindow() && !lock)
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

	glDeleteTextures(1, &g_tfTexture);
	glDeleteTextures(1, &g_volumeTexture);
	glDeleteTextures(1, &g_framebufferObject);

	// Test Code
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

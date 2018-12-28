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
#include "opengl/texture.h"
#include "openglutils.h"
#include "volume/virtualvolumehierachy.h"
#include "GL/glcorearb.h"
#include "framebuffer.h"


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
	FocusCamera g_camera{ ysl::Point3f{0.f,0.f,10.f} };
	ysl::Transform g_projMatrix;
	ysl::Transform g_orthoMatrix;
	ysl::ShaderProgram g_rayCastingShaderProgram;
	ysl::ShaderProgram g_positionShaderProgram;
	ysl::ShaderProgram g_quadsShaderProgram;
	ysl::ShaderProgram g_clearIntermediateProgram;
	ysl::Point2i g_lastMousePos;
	//std::shared_ptr<LargeVolumeCache> cache;

	std::unique_ptr<char[]> g_rawData;
	std::vector<ysl::RGBASpectrum> m_tfData{ 256 };
	//ysl::TransferFunction m_tfObject;

	ysl::Vector3f g_lightDirection;

	float step = 0.01;
	float ka = 1.0;
	float ks = 1.0;
	float kd = 1.0;
	float shininess = 50.0f;

	constexpr int pageTableBlockEntry = 16;
	const std::size_t missedBlockCapacity = 5000*sizeof(unsigned int);

	std::shared_ptr<OpenGLTexture> g_texVolume;

	std::shared_ptr<OpenGLTexture> g_texTransferFunction;

	std::shared_ptr<OpenGLFramebufferObject> g_framebuffer;


	std::shared_ptr<OpenGLTexture> g_texEntryPos;

	std::shared_ptr<OpenGLTexture> g_texExitPos;

	std::shared_ptr<OpenGLTexture> g_texDepth;

	std::shared_ptr<OpenGLTexture> g_texIntermediateResult;


	std::shared_ptr<OpenGLBuffer> g_proxyEBO;

	std::shared_ptr<OpenGLBuffer> g_proxyVBO;

	OpenGLVertexArrayObject g_proxyVAO;

	std::shared_ptr<OpenGLBuffer> g_rayCastingVBO;

	OpenGLVertexArrayObject g_rayCastingVAO;


	std::shared_ptr<OpenGLTexture> g_texPageDir;

	std::shared_ptr<OpenGLTexture> g_texPageTable;

	std::shared_ptr<OpenGLTexture> g_texCache;
	//ysl::Size3;

	std::shared_ptr<OpenGLBuffer> g_bufMissedHash;

	std::shared_ptr<OpenGLBuffer> g_bufMissedTable;

	std::shared_ptr<OpenGLBuffer> g_atomicCounter;

	/*unsigned int g_pageDirTexture;
	unsigned int g_pageTableTexture;
	unsigned int g_cacheTexture;
	unsigned int g_existBufferId;
	unsigned int g_missedBlockBufferId;*/

	std::unique_ptr<VolumeVirtualMemoryHierachy<pageTableBlockEntry, pageTableBlockEntry, pageTableBlockEntry>> g_largeVolumeData;

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
	g_texEntryPos->SetData(OpenGLTexture::RGBA32F, OpenGLTexture::RGBA, OpenGLTexture::Float32, x, y, 0, nullptr);
	g_texExitPos->SetData(OpenGLTexture::RGBA32F, OpenGLTexture::RGBA, OpenGLTexture::Float32, x, y, 0, nullptr);
	g_texDepth->SetData(OpenGLTexture::InternalDepthComponent, OpenGLTexture::ExternalDepthComponent, OpenGLTexture::Float32, x, y, 0, nullptr);
	g_texIntermediateResult->SetData(OpenGLTexture::RGBA32F, OpenGLTexture::RGBA, OpenGLTexture::Float32, x, y, 0, nullptr);

	// update raycastingtexture
	float windowSize[] =
	{
		0,0,x,0,0,y,0,y,x,0,x,y
	};

	g_rayCastingVBO->Bind();
	g_rayCastingVBO->AllocateFor(windowSize, sizeof(windowSize));
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

void initBlockExistsHash()
{

	const auto ptr = g_bufMissedHash->Map(OpenGLBuffer::WriteOnly);
	memset(ptr, 0, g_bufMissedHash->Size());
	g_bufMissedHash->Unmap();
	GL_ERROR_REPORT;

}

void initMissedBlockVector()
{
	const auto ptr = g_bufMissedTable->Map(OpenGLBuffer::WriteOnly);
	memset(ptr, 0, g_bufMissedTable->Size());
	g_bufMissedTable->Unmap();
	g_atomicCounter->BindBufferBase(3);
	unsigned int zero = 0;
	g_atomicCounter->AllocateFor(&zero, sizeof(unsigned int));
	GL_ERROR_REPORT;
}

/**
 * \brief  Returns \a true if cache missed, otherwise returns false
 */
bool CaptureAndHandleCacheMiss()
{
	// There is a run-time error at nvoglv64.dll
	const auto counters = static_cast<int*>(g_atomicCounter->Map(OpenGLBuffer::ReadOnly));
	const int count = counters[0];
	if (count == 0)
		return false;
	////For Debug
	//const auto hashPtr = static_cast<int*>(g_bufMissedHash->Map(OpenGLBuffer::ReadWrite));
	//g_bufMissedHash->Unmap();
	////

	const std::size_t blockSize = g_largeVolumeData->blockSize();
	std::vector<GlobalBlockAbstractIndex> hits;
	const auto ptr = static_cast<int*>(g_bufMissedTable->Map(OpenGLBuffer::ReadWrite));
	int maxCache = 8;

	/// TODO:: The loop count should smaller than the number of block count in gpu cache
	for (int i = 0; i < count && i < 64; i++) 
	{
		int blockId = ptr[i];
		const auto p = g_largeVolumeData->blockData(blockId);
		GlobalBlockAbstractIndex index(blockId, xBlockCount, yBlockCount, zBlockCount);
		hits.push_back(index);
	}
	GL_ERROR_REPORT;
	g_bufMissedTable->Unmap();

	auto & pageTable = *g_largeVolumeData->PageTable;
	auto & LRUList = g_largeVolumeData->m_lruList;

	///TODO:: 
	g_texCache->Bind();
	//static int hitCount = 0;
	for (const auto & i : hits)
	{
		auto & pageTableEntry = pageTable(i.x, i.y, i.z);
		//pageTable(i.x, i.y, i.z).w = EntryFlag::Mapped;
		auto & last = g_largeVolumeData->m_lruList.back();
		pageTableEntry.w = EntryFlag::Mapped;		// Map the flag of page table entry

		// last.second is the cache block index
		const auto xInCache = last.second.x;
		const auto yInCache = last.second.y;
		const auto zInCache = last.second.z;
		pageTableEntry.x = xInCache;			// fill the page table entry
		pageTableEntry.y = yInCache;
		pageTableEntry.z = zInCache;


		if(last.first.x != -1)
		{
			pageTable(last.first.x, last.first.y, last.first.z).w = EntryFlag::Unmapped;
			//ysl::Log("Pull [%d %d %d] from cache.\n", last.first.x, last.first.y, last.first.z);
		}

		last.first.x = i.x;
		last.first.y = i.y;
		last.first.z = i.z;
		LRUList.splice(LRUList.begin(), LRUList, --LRUList.end());		// move from tail to head, LRU policy
		auto d = g_largeVolumeData->blockData(i);
		g_texCache->SetSubData(OpenGLTexture::RED, OpenGLTexture::UInt8,xInCache,blockSize,yInCache,blockSize,zInCache,blockSize,d);
	}

	g_texCache->Unbind();
	GL_ERROR_REPORT;
	// update page table
	g_texPageTable->Bind();
	g_texPageTable->SetData(OpenGLTexture::RGBA32UI, OpenGLTexture::RGBAInteger, OpenGLTexture::UInt32, pageTableX, pageTableY, pageTableZ, g_largeVolumeData->PageTable->Data());
	g_texPageTable->Unbind();
	GL_ERROR_REPORT;
	return true;
}


void renderLoop()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	// clear intermediate result
	g_framebuffer->Bind();
	g_proxyVAO.bind();
	g_clearIntermediateProgram.bind();
	g_clearIntermediateProgram.setUniformValue("projMatrix", g_projMatrix.Matrix());
	g_clearIntermediateProgram.setUniformValue("worldMatrix", ysl::Transform{}.Matrix());
	g_clearIntermediateProgram.setUniformValue("viewMatrix", g_camera.view().Matrix());
	glDrawBuffer(GL_COLOR_ATTACHMENT2);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	GL_ERROR_REPORT;
	// Cull face
	g_positionShaderProgram.bind();
	g_positionShaderProgram.setUniformValue("projMatrix", g_projMatrix.Matrix());
	g_positionShaderProgram.setUniformValue("worldMatrix", ysl::Transform{}.Matrix());
	g_positionShaderProgram.setUniformValue("viewMatrix", g_camera.view().Matrix());
	GL_ERROR_REPORT;
	glDrawBuffer(GL_COLOR_ATTACHMENT0);					// Draw into attachment 0
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	GL_ERROR_REPORT;
	// Draw frontGL_ERROR_REPORT;
	glDepthFunc(GL_LESS);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	GL_ERROR_REPORT;
	// Draw Back
	glDrawBuffer(GL_COLOR_ATTACHMENT1);					// Draw into attachment 1
	glClear(GL_COLOR_BUFFER_BIT);						// Do not clear depth buffer here.
	glDepthFunc(GL_GREATER);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glDepthFunc(GL_LESS);
	// loop begin
	glDrawBuffer(GL_COLOR_ATTACHMENT2);		// Draw intermediate result into attachment 2
	glDepthFunc(GL_LESS);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);

	// init gpu hash table and missed cache block id
	//GLsync s;
	int maxLoopCount = 0;
	do
	{
		initBlockExistsHash();
		initMissedBlockVector();
		g_rayCastingShaderProgram.bind();
		//g_rayCastingShaderProgram.setUniformValue("totalPageDirSize", ysl::Vector3i{ pageDirX,pageDirY,pageDirZ });
		g_rayCastingShaderProgram.setUniformValue("totalPageTableSize", ysl::Vector3i{ pageTableX,pageTableY,pageTableZ });
		g_rayCastingShaderProgram.setUniformValue("blockDataSizeNoRepeat", ysl::Vector3i{ blockDataSize - 2 * repeat,blockDataSize - 2 * repeat,blockDataSize - 2 * repeat });
		g_rayCastingShaderProgram.setUniformValue("volumeDataSizeNoRepeat", ysl::Vector3i{ originalDataWidth,originalDataDepth,originalDataDepth });
		g_rayCastingShaderProgram.setUniformValue("pageTableBlockEntrySize", ysl::Vector3i{ pageTableBlockEntry ,pageTableBlockEntry ,pageTableBlockEntry });
		g_rayCastingShaderProgram.setUniformValue("repeatOffset", ysl::Vector3i{ repeat,repeat,repeat });
		g_rayCastingShaderProgram.setUniformSampler("cacheVolume", OpenGLTexture::TextureUnit5, *g_texCache);

		g_rayCastingShaderProgram.setUniformValue("viewMatrix", g_camera.view().Matrix());
		g_rayCastingShaderProgram.setUniformValue("orthoMatrix", g_orthoMatrix.Matrix());
		g_rayCastingShaderProgram.setUniformSampler("texVolume", OpenGLTexture::TextureUnit3, *g_texVolume);
		g_rayCastingShaderProgram.setUniformSampler("texStartPos", OpenGLTexture::TextureUnit0, *g_texEntryPos);
		g_rayCastingShaderProgram.setUniformSampler("texEndPos", OpenGLTexture::TextureUnit1, *g_texExitPos);
		g_rayCastingShaderProgram.setUniformSampler("texTransfunc", OpenGLTexture::TextureUnit2, *g_texTransferFunction);
		g_rayCastingShaderProgram.setUniformSampler("texIntermediateResult", OpenGLTexture::TextureUnit4, *g_texIntermediateResult);
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
		
		glDrawArrays(GL_TRIANGLES, 0, 6);

	/*	std::stringstream ss;
		ss << maxLoopCount;
		std::string str;
		ss >> str;
		g_texIntermediateResult->SaveAsImage("C:\\Users\\ysl\\Desktop\\debugimage\\result"+str+".jpg");
		g_texEntryPos->SaveAsImage("C:\\Users\\ysl\\Desktop\\debugimage\\entry" + str + ".jpg");*/
		maxLoopCount++;

	} while (CaptureAndHandleCacheMiss());

	ysl::Log("Render pass per frame:%d.\n", maxLoopCount);
	//for (int i = 0; i < 4; i++)
	//{
	//	CaptureAndHandleCacheMiss();
	//}
	g_framebuffer->Unbind();
	glDepthFunc(GL_LESS);
	g_quadsShaderProgram.bind();
	g_quadsShaderProgram.setUniformValue("viewMatrix", g_camera.view().Matrix());
	g_quadsShaderProgram.setUniformValue("orthoMatrix", g_orthoMatrix.Matrix());
	g_quadsShaderProgram.setUniformSampler("resTex", OpenGLTexture::TextureUnit2, *g_texIntermediateResult);
	g_rayCastingVAO.bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 6);			// Draw into default framebuffer
	glDisable(GL_DEPTH_TEST);
	g_quadsShaderProgram.unbind();
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

	std::shared_ptr<GLFWwindow> window{
		glfwCreateWindow(initialWidth, initialHeight, "Mixed Render Engine", NULL, NULL),
		[](GLFWwindow*window) {glfwDestroyWindow(window); } };

	glfwMakeContextCurrent(window.get());
	glfwSwapInterval(1); // Enable vsync
	gl3wInit();

	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO & io = ImGui::GetIO();
	(void)io;

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	ImGui_ImplGlfw_InitForOpenGL(window.get(), true);
	ImGui_ImplOpenGL3_Init();

	// Setup style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();


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
			g_texTransferFunction->SetData(OpenGLTexture::RGBA32F, OpenGLTexture::RGBA, OpenGLTexture::Float32, 256, 0, 0, m_tfData.data());
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

	// Ray casting in gpu
	g_rayCastingShaderProgram.create();
	g_rayCastingShaderProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\blockraycasting_v.glsl", ysl::ShaderProgram::ShaderType::Vertex);
	g_rayCastingShaderProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\blockraycasting_f.glsl", ysl::ShaderProgram::ShaderType::Fragment);
	g_rayCastingShaderProgram.link();

	// Generate entry and exit position
	g_positionShaderProgram.create();
	g_positionShaderProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\position_v.glsl", ysl::ShaderProgram::ShaderType::Vertex);
	g_positionShaderProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\position_f.glsl", ysl::ShaderProgram::ShaderType::Fragment);
	g_positionShaderProgram.link();

	//  Draw the result texture into the default framebuffer
	g_quadsShaderProgram.create();
	g_quadsShaderProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\quadsshader_v.glsl", ysl::ShaderProgram::ShaderType::Vertex);
	g_quadsShaderProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\quadsshader_f.glsl", ysl::ShaderProgram::ShaderType::Fragment);
	g_quadsShaderProgram.link();

	// Initialize the result texture
	g_clearIntermediateProgram.create();
	g_clearIntermediateProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\clear_v.glsl", ysl::ShaderProgram::ShaderType::Vertex);
	g_clearIntermediateProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\clear_f.glsl", ysl::ShaderProgram::ShaderType::Fragment);
	g_clearIntermediateProgram.link();

	//static float g_vertices[] =
	//{
	//	-0.5f, -0.5f, 0.0f,
	//	0.5f, -0.5f, 0.0f,
	//	0.0f,  0.5f, 0.0f
	//};

	// vao vbo
	GL_ERROR_REPORT;
	g_proxyVAO.create();
	g_proxyVAO.bind();

	g_proxyVBO = std::make_shared<OpenGLBuffer>(OpenGLBuffer::VertexArrayBuffer);
	g_proxyVBO->Bind();
	g_proxyVBO->AllocateFor(g_proxyGeometryVertices, sizeof(g_proxyGeometryVertices));

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(sizeof(g_proxyGeometryVertices) / 2));
	GL_ERROR_REPORT;
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));
	GL_ERROR_REPORT;

	g_proxyEBO = std::make_shared<OpenGLBuffer>(OpenGLBuffer::ElementArrayBuffer);
	g_proxyEBO->Bind();
	g_proxyEBO->AllocateFor(g_proxyGeometryVertexIndices, sizeof(g_proxyGeometryVertexIndices));
	GL_ERROR_REPORT;

	g_proxyVAO.unbind();

	g_rayCastingVAO.create();
	g_rayCastingVAO.bind();

	//g_rayCastingVBO.create();
	g_rayCastingVBO = std::make_shared<OpenGLBuffer>(OpenGLBuffer::VertexArrayBuffer);
	g_rayCastingVBO->Bind();
	g_rayCastingVBO->AllocateFor(nullptr, 6 * sizeof(ysl::Point2f));

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<void*>(0));

	g_rayCastingVAO.unbind();

	// texture

	std::string dataFileName;
	//std::cin >> dataFileName;
	dataFileName = "D:\\scidata\\abc\\s1_512_512_512.raw";

	std::size_t width, height, depth;
	width = 512; height = 512, depth = 512;
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

	g_largeVolumeData.reset(new VolumeVirtualMemoryHierachy<pageTableBlockEntry, pageTableBlockEntry, pageTableBlockEntry>(lvdFileName));

	//largeVolumeData.m_pageDir->Width();

	pageDirX = g_largeVolumeData->PageDir->Width();
	pageDirY = g_largeVolumeData->PageDir->Height();
	pageDirZ = g_largeVolumeData->PageDir->Depth();

	pageTableX = g_largeVolumeData->PageTable->Width();
	pageTableY = g_largeVolumeData->PageTable->Height();
	pageTableZ = g_largeVolumeData->PageTable->Depth();

	cacheWidth = g_largeVolumeData->CacheSize().x;
	cacheHeight = g_largeVolumeData->CacheSize().y;
	cacheDepth = g_largeVolumeData->CacheSize().z;

	repeat = g_largeVolumeData->repeat();
	blockDataSize = g_largeVolumeData->blockSize();
	originalDataWidth = g_largeVolumeData->originalWidth();
	originalDataHeight = g_largeVolumeData->originalHeight();
	originalDataDepth = g_largeVolumeData->originalDepth();

	xBlockCount = g_largeVolumeData->xBlockCount();
	yBlockCount = g_largeVolumeData->yBlockCount();
	zBlockCount = g_largeVolumeData->zBlockCount();

	const std::size_t totalBlockCountBytes = std::size_t(xBlockCount) * std::size_t(yBlockCount)*std::size_t(zBlockCount) * sizeof(int);

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
	std::cout << "data size:" << g_largeVolumeData->width() << std::endl;
	std::cout << "original data x:" << originalDataWidth << std::endl;
	std::cout << "original data y:" << originalDataHeight << std::endl;
	std::cout << "original data z:" << originalDataDepth << std::endl;

	std::cout << "BlockSize:" << g_largeVolumeData->blockSize() << std::endl;


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);		// This is important

	g_texVolume = OpenGLTexture::CreateTexture3D(OpenGLTexture::R16F,
		OpenGLTexture::Linear,
		OpenGLTexture::Linear,
		OpenGLTexture::ClampToEdge,
		OpenGLTexture::ClampToEdge,
		OpenGLTexture::ClampToEdge,
		OpenGLTexture::RED,
		OpenGLTexture::UInt8,
		width,
		height,
		depth,
		g_rawData.get());


	// page dir

	g_texPageDir = OpenGLTexture::CreateTexture3D(OpenGLTexture::RGBA32UI, OpenGLTexture::Linear,
		OpenGLTexture::Linear, OpenGLTexture::ClampToEdge,
		OpenGLTexture::ClampToEdge,
		OpenGLTexture::ClampToEdge,
		OpenGLTexture::RGBAInteger,
		OpenGLTexture::UInt32, pageDirX, pageDirY, pageDirZ, g_largeVolumeData->PageDir->Data());
	g_texPageDir->BindToDataImage(0, 0, false, 0, OpenGLTexture::Read, OpenGLTexture::RGBA32UI);
	GL_ERROR_REPORT;
	// page table
	g_texPageTable = OpenGLTexture::CreateTexture3D(OpenGLTexture::RGBA32UI, OpenGLTexture::Linear,
		OpenGLTexture::Linear, OpenGLTexture::ClampToEdge,
		OpenGLTexture::ClampToEdge,
		OpenGLTexture::ClampToEdge,
		OpenGLTexture::RGBAInteger,
		OpenGLTexture::UInt32, pageTableX, pageTableY, pageTableZ, g_largeVolumeData->PageTable->Data());
	// Bind to iimage3D
	g_texPageTable->BindToDataImage(1, 0, false, 0, OpenGLTexture::Read, OpenGLTexture::RGBA32UI);

	// allocate for volume data block cache
	g_texCache = OpenGLTexture::CreateTexture3D(OpenGLTexture::R16F,
		OpenGLTexture::Linear,
		OpenGLTexture::Linear, 
		OpenGLTexture::ClampToEdge,
		OpenGLTexture::ClampToEdge, 
		OpenGLTexture::ClampToEdge,
		OpenGLTexture::RED,
		OpenGLTexture::UInt8,
		cacheWidth,
		cacheHeight,
		cacheDepth, nullptr);

	// For Debugging, texCache should be fully filled for a small data set.
	//{
	//	const auto blockSize = g_largeVolumeData->blockSize();
	//	g_texCache->Bind();
	//	for(auto z = 0 ; z< g_largeVolumeData->zBlockCount();z++)
	//		for(auto y = 0 ;y<g_largeVolumeData->yBlockCount();y++)
	//			for(auto x = 0 ;x<g_largeVolumeData->xBlockCount();x++)
	//			{
	//				g_texCache->SetSubData(OpenGLTexture::RED,
	//					OpenGLTexture::UInt8,
	//					x*blockSize, blockSize,
	//					y*blockSize, blockSize,
	//					z*blockSize, blockSize,
	//					g_largeVolumeData->blockData(x, y, z));
	//				GL_ERROR_REPORT;
	//				ysl::Log("Put the block at 3d texture position:[%d, %d, %d]\n", x*blockSize, y*blockSize, z*blockSize);
	//				
	//			}

	//	g_texCache->Unbind();
	//}

	g_bufMissedHash = std::make_shared<OpenGLBuffer>(OpenGLBuffer::ShaderStorageBuffer, OpenGLBuffer::DynamicDraw);
	g_bufMissedHash->AllocateFor(nullptr, totalBlockCountBytes);
	g_bufMissedHash->BindBufferBase(0);

	g_bufMissedTable = std::make_shared<OpenGLBuffer>(OpenGLBuffer::ShaderStorageBuffer, OpenGLBuffer::DynamicDraw);
	g_bufMissedTable->AllocateFor(nullptr, missedBlockCapacity);
	g_bufMissedTable->BindBufferBase(1);

	g_atomicCounter = std::make_shared<OpenGLBuffer>(OpenGLBuffer::AtomicCounterBuffer,
		OpenGLBuffer::DynamicCopy);
	g_atomicCounter->AllocateFor(nullptr,sizeof(GLuint));
	g_atomicCounter->BindBufferBase(3);
	g_atomicCounter->Unbind();
	GL_ERROR_REPORT;
	// shader program
	g_rayCastingShaderProgram.bind();
	GL_ERROR_REPORT;
	//g_rayCastingShaderProgram.setUniformSampler("cacheVolume", OpenGLTexture::TextureUnit4, *g_texCache);
	g_rayCastingShaderProgram.setUniformValue("totalPageDirSize", ysl::Vector3i{ pageDirX,pageDirY,pageDirZ });
	g_rayCastingShaderProgram.setUniformValue("totalPageTableSize", ysl::Vector3i{ pageTableX,pageTableY,pageTableZ });
	g_rayCastingShaderProgram.setUniformValue("blockDataSize", ysl::Vector3i{ blockDataSize - 2 * repeat,blockDataSize - 2 * repeat,blockDataSize - 2 * repeat });
	g_rayCastingShaderProgram.setUniformValue("volumeDataSize", ysl::Vector3i{ originalDataWidth,originalDataDepth,originalDataDepth });
	g_rayCastingShaderProgram.setUniformValue("pageTableBlockEntrySize", ysl::Vector3i{ pageTableBlockEntry ,pageTableBlockEntry ,pageTableBlockEntry });
	g_rayCastingShaderProgram.setUniformValue("repeatSize", ysl::Vector3i{ repeat,repeat,repeat });
	g_rayCastingShaderProgram.unbind();

	g_texTransferFunction = OpenGLTexture::CreateTexture1D(OpenGLTexture::RGBA32F,
		OpenGLTexture::Linear, OpenGLTexture::Linear, OpenGLTexture::ClampToEdge, OpenGLTexture::RGBA, OpenGLTexture::Float32, 256, nullptr);

	g_texEntryPos = OpenGLTexture::CreateTexture2DRect(OpenGLTexture::RGBA32F,
		OpenGLTexture::Linear,
		OpenGLTexture::Linear,
		OpenGLTexture::ClampToEdge,
		OpenGLTexture::ClampToEdge,
		OpenGLTexture::RGBA,
		OpenGLTexture::Float32,
		initialWidth,
		initialHeight,
		nullptr);
	g_texEntryPos->BindToDataImage(2, 0, false, 0, OpenGLTexture::ReadAndWrite, OpenGLTexture::RGBA32F);



	g_texExitPos = OpenGLTexture::CreateTexture2DRect(OpenGLTexture::RGBA32F,
		OpenGLTexture::Linear,
		OpenGLTexture::Linear,
		OpenGLTexture::ClampToEdge,
		OpenGLTexture::ClampToEdge,
		OpenGLTexture::RGBA,
		OpenGLTexture::Float32,
		initialWidth,
		initialHeight,
		nullptr);

	g_texIntermediateResult = OpenGLTexture::CreateTexture2DRect(OpenGLTexture::RGBA32F,
		OpenGLTexture::Linear,
		OpenGLTexture::Linear,
		OpenGLTexture::ClampToEdge,
		OpenGLTexture::ClampToEdge,
		OpenGLTexture::RGBA,
		OpenGLTexture::Float32,
		initialWidth,
		initialHeight,
		nullptr);


	g_texDepth = OpenGLTexture::CreateTexture2DRect(OpenGLTexture::InternalDepthComponent,
		OpenGLTexture::Linear,
		OpenGLTexture::Linear,
		OpenGLTexture::ClampToEdge,
		OpenGLTexture::ClampToEdge,
		OpenGLTexture::ExternalDepthComponent,
		OpenGLTexture::Float32,
		initialWidth,
		initialHeight,
		nullptr);

	g_framebuffer = std::shared_ptr<OpenGLFramebufferObject>(new OpenGLFramebufferObject);
	g_framebuffer->Bind();
	g_framebuffer->AttachTexture(OpenGLFramebufferObject::ColorAttachment0, g_texEntryPos);
	g_framebuffer->AttachTexture(OpenGLFramebufferObject::ColorAttachment1, g_texExitPos);
	g_framebuffer->AttachTexture(OpenGLFramebufferObject::ColorAttachment2, g_texIntermediateResult);
	g_framebuffer->AttachTexture(OpenGLFramebufferObject::DepthAttachment, g_texDepth);
	g_framebuffer->CheckFramebufferStatus();
	g_framebuffer->Unbind();
	GL_ERROR_REPORT;
	//init 
	initBlockExistsHash();
	initMissedBlockVector();
	GL_ERROR_REPORT;

	{
		

		ysl::TransferFunction m_tfObject;
		m_tfObject.read("d:\\scidata\\std_tf1d.TF1D");

		m_tfObject.FetchData(m_tfData.data(), 256);
		g_texTransferFunction->SetData(OpenGLTexture::RGBA32F, OpenGLTexture::RGBA, OpenGLTexture::Float32, 256, 0, 0, m_tfData.data());
	}


	// Main loop
	while (!glfwWindowShouldClose(window.get()))
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
		ImGui::SliderFloat("ka", &ka, 0.0f, 1.0f);
		ImGui::SliderFloat("kd", &kd, 0.0f, 1.0f);
		ImGui::SliderFloat("ks", &ks, 0.0f, 1.0f);
		ImGui::SliderFloat("shininess", &shininess, 0.0f, 50.f);
		if (ImGui::Button("lock"))
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
		glfwMakeContextCurrent(window.get());
		glfwGetFramebufferSize(window.get(), &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		// User's render is here
		renderLoop();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwMakeContextCurrent(window.get());
		glfwSwapBuffers(window.get());
	}



	// Test Code
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	//glfwDestroyWindow(window);


	glfwTerminate();

	system("pause");

	return 0;
}


#ifndef _OPENGLRENDERER_H_
#define _OPENGLRENDERER_H_

#include "../cameras/camera.h"
#include "../opengl/shader.h"
#include "../opengl/framebuffer.h"
#include "../opengl/openglbuffer.h"
#include "../opengl/openglvertexarrayobject.h"

//
//class OpenGLRenderLoop {
//
//
//
//public:
//	int exec();
//
//};
//
//class OpenGLRenderer
//{
//	
//public:
//	OpenGLRenderer();
//	virtual void InitializeResources() = 0;
//	virtual void RenderLoop() = 0;
//	virtual void ImGuiUpdate() = 0;
//	virtual void DestroyResources() = 0;
//	virtual ~OpenGLRenderer();
//};
//
//class VolumeRenderer
//{
//	FocusCamera g_camera{ ysl::Point3f{0.f,0.f,10.f} };
//	ysl::Transform g_projMatrix;
//	ysl::Transform g_orthoMatrix;
//	ysl::ShaderProgram g_rayCastingShaderProgram;
//	ysl::ShaderProgram g_positionShaderProgram;
//	ysl::ShaderProgram g_quadsShaderProgram;
//	ysl::ShaderProgram g_clearIntermediateProgram;
//	ysl::Point2i g_lastMousePos;
//
//	std::unique_ptr<char[]> g_rawData;
//	std::vector<ysl::RGBASpectrum> m_tfData{ 256 };
//
//	ysl::Vector3f g_lightDirection;
//
//	float step = 0.01;
//	float ka = 1.0;
//	float ks = 1.0;
//	float kd = 1.0;
//	float shininess = 50.0f;
//
//	constexpr static int pageTableBlockEntry = 16;
//	const std::size_t missedBlockCapacity = 5000 * sizeof(unsigned int);
//
//	std::shared_ptr<OpenGLTexture> g_texVolume;
//
//	std::shared_ptr<OpenGLTexture> g_texTransferFunction;
//
//	std::shared_ptr<OpenGLFramebufferObject> g_framebuffer;
//
//
//	std::shared_ptr<OpenGLTexture> g_texEntryPos;
//
//	std::shared_ptr<OpenGLTexture> g_texExitPos;
//
//	std::shared_ptr<OpenGLTexture> g_texDepth;
//
//	std::shared_ptr<OpenGLTexture> g_texIntermediateResult;
//
//
//	std::shared_ptr<OpenGLBuffer> g_proxyEBO;
//
//	std::shared_ptr<OpenGLBuffer> g_proxyVBO;
//
//	OpenGLVertexArrayObject g_proxyVAO;
//
//	std::shared_ptr<OpenGLBuffer> g_rayCastingVBO;
//
//	OpenGLVertexArrayObject g_rayCastingVAO;
//
//
//	std::shared_ptr<OpenGLTexture> g_texPageDir;
//
//	std::shared_ptr<OpenGLTexture> g_texPageTable;
//
//	std::shared_ptr<OpenGLTexture> g_texCache;
//	//ysl::Size3;
//
//	std::shared_ptr<OpenGLBuffer> g_bufMissedHash;
//
//	std::shared_ptr<OpenGLBuffer> g_bufMissedTable;
//
//	std::shared_ptr<OpenGLBuffer> g_atomicCounter;
//
//	/*unsigned int g_pageDirTexture;
//	unsigned int g_pageTableTexture;
//	unsigned int g_cacheTexture;
//	unsigned int g_existBufferId;
//	unsigned int g_missedBlockBufferId;*/
//
//	std::unique_ptr<VolumeVirtualMemoryHierachy<pageTableBlockEntry, pageTableBlockEntry, pageTableBlockEntry>> g_largeVolumeData;
//
//public:
//	VolumeRenderer()
//	{
//		
//	}
//
//};

#endif

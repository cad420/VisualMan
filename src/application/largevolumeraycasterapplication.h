

#ifndef _LARGEVOLUMERAYCASTERAPPLICATION_H_
#define _LARGEVOLUMERAYCASTERAPPLICATION_H_

#include "../application/guiapplication.h"
#include "../volume/virtualvolumehierachy.h"
#include "../opengl/openglbuffer.h"
#include "../opengl/openglvertexarrayobject.h"
#include "../utility/timer.h"
#include "../cameras/camera.h"
#include "../opengl/shader.h"
#include "../gui/transferfunctionwidget.h"
#include "gpucacheblockmanager.h"




//#define COUNT_VALID_BLOCK


namespace ysl
{
	namespace app
	{

		constexpr int pageTableBlockEntry = 16;
		class LargeVolumeRayCaster:public ImGuiApplication

		{
		public:
			LargeVolumeRayCaster(int argc, char ** argv, int w, int h,const std::string & fileName);
		protected:
			void MousePressEvent(MouseEvent* event) override;
			void MouseMoveEvent(MouseEvent* event) override;
			void MouseReleaseEvent(MouseEvent* e) override;
			void WindowResizeEvent(ResizeEvent* event) override;
			void InitializeOpenGLResources() override;
			void DrawImGui() override;
			void RenderLoop() override;
		private:
			void OpenGLConfiguration();
			void InitGPUPageTableBuffer();
			void InitGPUBlockCacheTexture();		// texCache
			void InitializeProxyGeometry();
			void InitializeResource();
			void InitTransferFunctionTexture();
			void InitRayCastingTexture();
			void InitializeShaders();
			void SetShaderUniforms();
			bool CaptureAndHandleCacheFault();

			FocusCamera camera;
			ysl::Transform projMatrix;
			ysl::Transform orthoMatrix;
			ysl::Transform modelMatrix;
			ysl::ShaderProgram rayCastingShaderProgram;
			ysl::ShaderProgram positionShaderProgram;
			ysl::ShaderProgram quadsShaderProgram;
			ysl::ShaderProgram clearIntermediateProgram;
			ysl::Point2i lastMousePos;
			//std::unique_ptr<char[]> g_rawData;
			ysl::TransferFunction tfObject;
			std::vector<ysl::RGBASpectrum> tfData;
			ysl::Vector3f lightDirection;

			float step;
			float ka;
			float ks;
			float kd;
			float shininess;

			std::shared_ptr<OpenGLTexture> texTransferFunction;
			std::shared_ptr<OpenGLFramebufferObject> framebuffer;
			std::shared_ptr<OpenGLTexture> texEntryPos;
			std::shared_ptr<OpenGLTexture> texExitPos;
			std::shared_ptr<OpenGLTexture> texDepth;
			std::shared_ptr<OpenGLTexture> texIntermediateResult;

			std::shared_ptr<OpenGLBuffer> proxyEBO;
			std::shared_ptr<OpenGLBuffer> proxyVBO;
			OpenGLVertexArrayObject proxyVAO;
			std::shared_ptr<OpenGLBuffer> rayCastingVBO;
			OpenGLVertexArrayObject rayCastingVAO;

			// One LOD Data
			ysl::Size3 gpuCacheBlockSize;				
			std::shared_ptr<OpenGLTexture> texPageTable;
			std::shared_ptr<HashBasedGPUCacheFaultHandler> cacheFaultHandler; // Belong to Client-end memory
			std::shared_ptr<PingPongTransferManager> pingpongTransferManager;
			std::shared_ptr<GPUVolumeDataCache> texCache;			 // Client-end memory
			CPUVolumeDataCache largeVolumeCache;		// Server-end memory
			VirtualMemoryManager pageTableManager;		// Server-end memory

#ifdef COUNT_VALID_BLOCK
			std::shared_ptr<OpenGLBuffer> bufMissedHash;
			std::shared_ptr<OpenGLBuffer> atomicCounter;
			void InitCounter(int capacity);
			int ResetCounter();
#endif
			//int g_pageTableX;
			//int g_pageTableY;
			//int g_pageTableZ;

			//int g_cacheWidth;
			//int g_cacheHeight;
			//int g_cacheDepth;

			int g_repeat;
			int g_blockDataSize;
			int g_originalDataWidth;
			int g_originalDataHeight;
			int g_originalDataDepth;

			int g_initialWidth ,g_initialHeight;

			int g_blockUploadMicroSecondsPerFrame;
			int g_uploadBlockCountPerFrame;
			int g_missingCacheCountPerFrame;
			int g_renderPassPerFrame;

			int g_blockBytes;
		
			std::shared_ptr<imgui::TransferFunctionWidget> TFWidget;
		};
	}
}

#endif

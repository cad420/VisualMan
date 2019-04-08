

#ifndef _LARGEVOLUMERAYCASTERAPPLICATION_H_
#define _LARGEVOLUMERAYCASTERAPPLICATION_H_

#include "../application/guiapplication.h"
#include "../opengl/virtualvolumehierachy.h"
#include "../opengl/openglbuffer.h"
#include "../opengl/openglvertexarrayobject.h"
#include "../cameras/camera.h"
#include "../opengl/shader.h"
#include "../gui/transferfunctionwidget.h"
#include "../opengl/gpucachefaulthandler.h"
#include "../opengl/gpuvolumedatacache.h"
#include "../opengl/gpucacheblockmanager.h"
#include "../opengl/gpupagetable.h"
#include "lodaggregate.h"

//#define COUNT_VALID_BLOCK

//#define USE_AGGREGATE



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
			void InitializeProxyGeometry();
			void InitializeResource();
			void InitTransferFunctionTexture();
			void InitRayCastingTexture();
			void InitializeShaders();
			void SetShaderUniforms();
			bool CaptureAndHandleCacheFault();
			int CalcLOD();

			FocusCamera camera;
			Transform projMatrix;
			Transform orthoMatrix;
			Transform modelMatrix;
			ShaderProgram rayCastingShaderProgram;
			ShaderProgram positionShaderProgram;
			ShaderProgram quadsShaderProgram;
			ShaderProgram clearIntermediateProgram;
			Point2i lastMousePos;
			TransferFunction tfObject;
			std::vector<ysl::RGBASpectrum> tfData;
			Vector3f lightDirection;

			float step;
			float ka;
			float ks;
			float kd;
			float shininess;

			//static constexpr std::size_t missedBlockCapacity = 5000 * sizeof(unsigned int);

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
			//ysl::Size3 gpuCacheBlockSize;		

#ifndef USE_AGGREGATE

			std::string fileName;
			//std::shared_ptr<HashBasedGPUCacheFaultHandler> cacheFaultHandler; // Belong to Client-end memory
			//std::shared_ptr<PingPongTransferManager> pingpongTransferManager;
			//std::shared_ptr<GPUVolumeDataCache> texCache;			 // Client-end memory
			//std::shared_ptr<CPUVolumeDataCache> largeVolumeCache;
			//std::shared_ptr<PageTableManager> pageTableManager;		// Server-end memory
#endif
			std::vector<std::shared_ptr<LODAggregate>> aggregates;
			int currentLod;

			//std::shared_ptr<LODAggregate> aggregate;
			SHADERBINDINGPOINT SBP;
			void InitializeLODs(const std::vector<std::string> fileNames);
			void PrintAggregateInfo(const std::shared_ptr<LODAggregate> & aggr);

#ifdef COUNT_VALID_BLOCK
			std::shared_ptr<OpenGLBuffer> bufMissedHash;
			std::shared_ptr<OpenGLBuffer> atomicCounter;
			void InitCounter(int capacity);
			int ResetCounter();
#endif

			//int windowWidth;
			//int windowHeight;
			Vec2i windowSize;
			std::shared_ptr<imgui::TransferFunctionWidget> TFWidget;
		};
	}
}

#endif

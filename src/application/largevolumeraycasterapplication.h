

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


namespace ysl
{
	namespace app
	{

		constexpr int pageTableBlockEntry = 16;
		class LargeVolumeRayCaster:public ImGuiApplication,
								   public VolumeVirtualMemoryHierarchy<pageTableBlockEntry, pageTableBlockEntry, pageTableBlockEntry>
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
			void InitGPUBlockCacheTexture();
			void initGPUCacheLRUPolicyList();
			void InitPingPongSwapPBO();
			void InitHashBuffer();
			void InitMissTableBuffer();
			void InitGPUAtomicCounter();

			void InitializeProxyGeometry();
			void InitializeResource();
			void InitTransferFunctionTexture();
			void InitRayCastingTexture();
		
			void InitializeShaders();
			void SetShaderUniforms();

			bool CaptureAndHandleCacheMiss();
			void InitBlockExistsHash();
			void InitMissedBlockVector();

			FocusCamera camera;
			ysl::Transform projMatrix;
			ysl::Transform orthoMatrix;
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

			static constexpr int pageTableBlockEntry = 16;
			Timer g_timer;
			Timer g_timer2;

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
			std::shared_ptr<OpenGLTexture> texPageTable;
			std::shared_ptr<OpenGLTexture> texCache;
			std::list<std::pair<PageTableEntryAbstractIndex, CacheBlockAbstractIndex>> g_lruList;
			std::shared_ptr<OpenGLBuffer> blockPingBuf;
			std::shared_ptr<OpenGLBuffer> blockPongBuf;
			std::vector<GlobalBlockAbstractIndex> hits;

			std::vector<int> posInCache;
			ysl::Size3 gpuCacheBlockSize;
			std::shared_ptr<OpenGLBuffer> bufMissedHash;
			std::shared_ptr<OpenGLBuffer> bufMissedTable;
			std::shared_ptr<OpenGLBuffer> atomicCounter;


			int g_pageTableX;
			int g_pageTableY;
			int g_pageTableZ;

			int g_cacheWidth;
			int g_cacheHeight;
			int g_cacheDepth;

			int g_repeat;
			int g_blockDataSize;
			int g_originalDataWidth;
			int g_originalDataHeight;
			int g_originalDataDepth;

			int g_xBlockCount;
			int g_yBlockCount;
			int g_zBlockCount;

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

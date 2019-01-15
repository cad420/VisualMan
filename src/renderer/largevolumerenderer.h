

#ifndef _LARGEVOLUMERENDERER_H_
#define _LARGEVOLUMERENDERER_H_

#include "../application/guiapplication.h"
#include "../opengl/shader.h"
#include "../cameras/camera.h"
#include "../volume/volume_utils.h"
#include "../utility/timer.h"
#include "../opengl/openglbuffer.h"
#include "../opengl/openglvertexarrayobject.h"
#include "../volume/virtualvolumehierachy.h"
#include "../opengl/openglutils.h"
#include "../../lib/gl3w/GL/gl3w.h"

namespace ysl
{
	namespace app
	{
		class LargeVolumeRenderApplication:public GuiApplication
		{
		public:
			LargeVolumeRenderApplication(int argc, char ** argv, int w, int h);
		protected:
			void InitializeOpenGLResources() override;
			void RenderLoop() override;
		private:
			void InitBlockExistsHash();
			void InitMissedBlockVector();
			void InitGPUCacheLRUPolicyList();
			bool CaptureAndHandleCacheMiss();
			void InitializeShaders();

			void InitializeProxyGeometry();
			void InitializeResource();

		private:
			FocusCamera g_camera{ ysl::Point3f{0.f,0.f,5.f} };
			ysl::Transform g_projMatrix;
			ysl::Transform g_orthoMatrix;
			ysl::ShaderProgram g_rayCastingShaderProgram;
			ysl::ShaderProgram g_positionShaderProgram;
			ysl::ShaderProgram g_quadsShaderProgram;
			ysl::ShaderProgram g_clearIntermediateProgram;
			ysl::Point2i g_lastMousePos;
			//std::unique_ptr<char[]> g_rawData;
			ysl::TransferFunction g_tfObject;
			std::vector<ysl::RGBASpectrum> g_tfData{ 256 };
			//std::string g_lvdFileName = "D:\\scidata\\abc\\sb__128_128_128_2_64.lvd";
			//std::string g_lvdFileName = "D:\\scidata\\abc\\sb__120_120_120_2_64.lvd";
			//std::string g_lvdFileName = "D:\\scidata\\abc\\sb__60_60_60_2_64.lvd";
			std::string g_lvdFileName = "C:\\data\\s1_480_480_480_2_64.lvd";
			//std::string g_lvdFileName = "C:\\data\\s1_3968_3968_3968_2_128.lvd";
			ysl::Vector3f g_lightDirection;
			float step = 0.001;
			float ka = 1.0;
			float ks = 1.0;
			float kd = 1.0;
			float shininess = 50.0f;

			static constexpr int pageTableBlockEntry = 16;
			Timer g_timer;
			Timer g_timer2;

			const std::size_t missedBlockCapacity = 5000 * sizeof(unsigned int);

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

			std::shared_ptr<OpenGLTexture> g_texPageTable;

			std::shared_ptr<OpenGLTexture> g_texCache;

			std::list<std::pair<PageTableEntryAbstractIndex, CacheBlockAbstractIndex>> g_lruList;

			std::shared_ptr<OpenGLBuffer> g_blockPingBuf;
			std::shared_ptr<OpenGLBuffer> g_blockPongBuf;
			unsigned int g_pingPBO;
			unsigned int g_pongPBO;
			char * g_pboPtr[2];


			std::vector<GlobalBlockAbstractIndex> g_hits;

			std::vector<int> g_posInCache;

			ysl::Size3 g_gpuCacheBlockSize{ 4,4,4 };

			std::shared_ptr<OpenGLBuffer> g_bufMissedHash;
			int * g_cacheHashPtr;

			std::shared_ptr<OpenGLBuffer> g_bufMissedTable;
			int * g_cacheMissTablePtr;

			std::shared_ptr<OpenGLBuffer> g_atomicCounter;
			int * g_atomicCounterPtr;

			//char * g_data;
			std::unique_ptr<VolumeVirtualMemoryHierachy<pageTableBlockEntry, pageTableBlockEntry, pageTableBlockEntry>> 
			g_largeVolumeData;

			int g_pageDirX;
			int g_pageDirY;
			int g_pageDirZ;

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

			int g_initialWidth = 800, g_initialHeight = 600;

			int g_blockUploadMicroSecondsPerFrame;
			int g_uploadBlockCountPerFrame;
			int g_missingCacheCountPerFrame;
			int g_renderPassPerFrame;

		};
	}
}


#endif

#include "largevolumeraycasterapplication.h"

#include "../application/event.h"
#include "../opengl/openglutils.h"
#include "../../lib/gl3w/GL/gl3w.h"

#include <algorithm>

const std::string tfName = R"(D:\scidata\tf1.tfi)";
//const std::string tfName = R"(D:\\subregion.1dt)";

namespace ysl
{
	namespace app
	{
		const static int g_proxyGeometryVertexIndices[] = { 1,3,7,1,7,5,0,4,6,0,6,2,2,6,7,2,7,3,0,1,5,0,5,4,4,5,7,4,7,6,0,2,3,0,3,1 };
		static const float xCoord = 1.0, yCoord = 1.0, zCoord = 1.0;
		static float g_proxyGeometryVertices[] = 
		{
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

		LargeVolumeRayCaster::LargeVolumeRayCaster(int argc, char** argv, int w, int h, const std::string& fileName) :
			ImGuiApplication(argc, argv, w, h),
		gpuCacheBlockSize{13,13,12},
		largeVolumeCache(fileName),
		pageTableManager(gpuCacheBlockSize,&largeVolumeCache)
		{
			camera = FocusCamera{ Point3f{0.f,0.f,5.f} };
			tfData.resize(256);

			g_initialWidth = 800, g_initialHeight = 600;
			step = 0.001;
			ka = 1.0;
			ks = 1.0;
			kd = 1.0;
			shininess = 50.0f;
		}

		void LargeVolumeRayCaster::MousePressEvent(MouseEvent* event)
		{
			lastMousePos = event->pos();
		}

		void LargeVolumeRayCaster::MouseMoveEvent(MouseEvent* event)
		{
			const auto & p = event->pos();
			// Update Camera
			float dx = p.x - lastMousePos.x;
			float dy = lastMousePos.y - p.y;
			if (dx == 0.0 && dy == 0.0)
				return;
			if ((event->buttons() & MouseEvent::LeftButton) && (event->buttons() & MouseEvent::RightButton))
			{
				const auto directionEx = camera.up()*dy + dx * camera.right();
				camera.movement(directionEx, 0.002);
			}
			else if (event->buttons() & MouseEvent::LeftButton)
			{
				camera.rotation(dx, dy);
			}
			else if (event->buttons() == MouseEvent::RightButton)
			{
				const auto directionEx = camera.front()*dy;
				camera.movement(directionEx, 0.01);
			}

			lastMousePos = p;
		}

		void LargeVolumeRayCaster::MouseReleaseEvent(MouseEvent* e)
		{

		}

		void LargeVolumeRayCaster::WindowResizeEvent(ResizeEvent* event)
		{
			const auto x = event->size().x;
			const auto y = event->size().y;
			orthoMatrix.SetGLOrtho(0, x, 0, y, -10, 100);

			//
			const auto aspect = static_cast<float>(x) / static_cast<float>(y);
			projMatrix.SetGLPerspective(45.0f, aspect, 0.01, 100);

			// Update frambuffer size
			texEntryPos->SetData(OpenGLTexture::RGBA32F, OpenGLTexture::RGBA, OpenGLTexture::Float32, x, y, 0, nullptr);
			texExitPos->SetData(OpenGLTexture::RGBA32F, OpenGLTexture::RGBA, OpenGLTexture::Float32, x, y, 0, nullptr);
			texDepth->SetData(OpenGLTexture::InternalDepthComponent, OpenGLTexture::ExternalDepthComponent, OpenGLTexture::Float32, x, y, 0, nullptr);
			texIntermediateResult->SetData(OpenGLTexture::RGBA32F, OpenGLTexture::RGBA, OpenGLTexture::Float32, x, y, 0, nullptr);

			// update raycastingtexture
			float windowSize[] =
			{
				0,0,x,0,0,y,0,y,x,0,x,y
			};

			rayCastingVBO->Bind();
			rayCastingVBO->AllocateFor(windowSize, sizeof(windowSize));
		}

		void LargeVolumeRayCaster::InitializeOpenGLResources()
		{
			InitializeResource();

			g_blockBytes = largeVolumeCache.BlockSize()*largeVolumeCache.BlockSize()*largeVolumeCache.BlockSize() * sizeof(char);


			TransferFunction::Callback cb = [this](TransferFunction * tf)
			{
				tf->FetchData(tfData.data(), 256);
				texTransferFunction->SetData(OpenGLTexture::RGBA32F,
					OpenGLTexture::RGBA,
					OpenGLTexture::Float32,
					256, 0, 0, tfData.data());
				GL_ERROR_REPORT;
			};

			TFWidget = std::make_shared<imgui::TransferFunctionWidget>(tfName);
			TFWidget->AddUpdateCallBack(cb);

			//AddWidget(TFWidget);
		}

		void LargeVolumeRayCaster::DrawImGui()
		{
			TFWidget->Draw();

		/*	ImGui::Begin("Control Panel");
			ImGui::Text("Page Table Size:[%d, %d, %d]", g_pageTableX, g_pageTableY, g_pageTableZ);
			ImGui::Text("Cache Block Count In CPU:[%d, %d, %d]", g_cacheWidth, g_cacheHeight, g_cacheDepth);
			ImGui::Text("Cache Block Count In GPU:[%d, %d, %d]", gpuCacheBlockSize.x, gpuCacheBlockSize.y, gpuCacheBlockSize.z);
			ImGui::Text("Cache Block Size:[%d]", BlockSize());
			ImGui::Text("Block Border:%d", g_repeat);
			ImGui::Text("Upload Block Count Per Frame:%d", g_uploadBlockCountPerFrame);
			ImGui::Text("Missing Cache Block Count Per Frame:%d", g_missingCacheCountPerFrame);
			ImGui::Text("Rendering pass count per frame:%d", g_renderPassPerFrame);
			const auto Gbs = double(g_blockBytes * g_uploadBlockCountPerFrame) / (1 << 30);
			const auto s = double(g_blockUploadMicroSecondsPerFrame) / 1000000;
			ImGui::Text("BandWidth: %f Gb/s", Gbs / s);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			ImGui::SliderFloat("step", &step, 0.001, 1.0);
			ImGui::SliderFloat("ka", &ka, 0.0f, 1.0f);
			ImGui::SliderFloat("kd", &kd, 0.0f, 1.0f);
			ImGui::SliderFloat("ks", &ks, 0.0f, 1.0f);
			ImGui::SliderFloat("shininess", &shininess, 0.0f, 50.f);
			ImGui::End();*/


		}

		void LargeVolumeRayCaster::RenderLoop()
		{

			glClear(GL_COLOR_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			// clear intermediate result
			framebuffer->Bind();
			proxyVAO.bind();
			// Cull face
			positionShaderProgram.bind();
			positionShaderProgram.setUniformValue("projMatrix", projMatrix.Matrix());
			positionShaderProgram.setUniformValue("worldMatrix", modelMatrix.Matrix());
			positionShaderProgram.setUniformValue("viewMatrix", camera.view().Matrix());

			glDrawBuffer(GL_COLOR_ATTACHMENT0);					// Draw into attachment 0
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			GL_ERROR_REPORT;
			// Draw front  GL_ERROR_REPORT;
			glDepthFunc(GL_LESS);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

			// Draw Back
			glDrawBuffer(GL_COLOR_ATTACHMENT1);					// Draw into attachment 1
			glClear(GL_COLOR_BUFFER_BIT);						// Do not clear depth buffer here.
			glDepthFunc(GL_GREATER);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			glDepthFunc(GL_LESS);

			// Clear
			clearIntermediateProgram.bind();
			clearIntermediateProgram.setUniformValue("projMatrix", projMatrix.Matrix());
			clearIntermediateProgram.setUniformValue("worldMatrix", ysl::Transform{}.Matrix());
			clearIntermediateProgram.setUniformValue("viewMatrix", camera.view().Matrix());
			//g_clearIntermediateProgram.setUniformSampler("texStartPos", OpenGLTexture::TextureUnit0, *g_texEntryPos);
			//g_clearIntermediateProgram.setUniformSampler("texEndPos", OpenGLTexture::TextureUnit1, *g_texExitPos);

			glDrawBuffer(GL_COLOR_ATTACHMENT2);
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

			// loop begin
			framebuffer->Unbind();
			glDepthFunc(GL_LESS);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);

			rayCastingShaderProgram.bind();

			rayCastingShaderProgram.setUniformSampler("cacheVolume", OpenGLTexture::TextureUnit5, *texCache);
			rayCastingShaderProgram.setUniformSampler("texStartPos", OpenGLTexture::TextureUnit0, *texEntryPos);
			rayCastingShaderProgram.setUniformSampler("texEndPos", OpenGLTexture::TextureUnit1, *texExitPos);
			rayCastingShaderProgram.setUniformSampler("texTransfunc", OpenGLTexture::TextureUnit2, *texTransferFunction);
			rayCastingShaderProgram.setUniformSampler("texIntermediateResult", OpenGLTexture::TextureUnit4, *texIntermediateResult);

			rayCastingShaderProgram.setUniformValue("viewMatrix", camera.view().Matrix());
			rayCastingShaderProgram.setUniformValue("orthoMatrix", orthoMatrix.Matrix());
			rayCastingShaderProgram.setUniformValue("step", step);
			rayCastingShaderProgram.setUniformValue("ka", ka);
			rayCastingShaderProgram.setUniformValue("ks", ks);
			rayCastingShaderProgram.setUniformValue("kd", kd);
			rayCastingShaderProgram.setUniformValue("shininess", shininess);
			rayCastingShaderProgram.setUniformValue("lightdir", lightDirection);

			//auto halfWay = g_lightDirection - g_camera.front();
			//if (halfWay.Length() > 1e-10) halfWay.Normalize();
			//g_rayCastingShaderProgram.setUniformValue("halfway", halfWay);


			rayCastingVAO.bind();

			g_renderPassPerFrame = 0;
			g_missingCacheCountPerFrame = 0;
			g_uploadBlockCountPerFrame = 0;
			g_blockUploadMicroSecondsPerFrame = 0;

			GL_ERROR_REPORT;
			do
			{
				//cacheFaultHandler->Reset();
				glDrawArrays(GL_TRIANGLES, 0, 6);
			} while (CaptureAndHandleCacheFault());
#ifdef COUNT_VALID_BLOCK
			std::cout <<"Valid Block:"<< ResetCounter() << std::endl;
#endif // COUNT_VALID_BLOCK

			// Draw final result quad texture on screen

			//framebuffer->Unbind();
			glDepthFunc(GL_LESS);
			quadsShaderProgram.bind();
			quadsShaderProgram.setUniformValue("viewMatrix", camera.view().Matrix());
			quadsShaderProgram.setUniformValue("orthoMatrix", orthoMatrix.Matrix());
			quadsShaderProgram.setUniformSampler("resTex", OpenGLTexture::TextureUnit2, *texIntermediateResult);

			rayCastingVAO.bind();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDrawArrays(GL_TRIANGLES, 0, 6);			// Draw into default framebuffer
			glDisable(GL_DEPTH_TEST);
			//glDisable(GL_BLEND);
			quadsShaderProgram.unbind();
		}

		void LargeVolumeRayCaster::OpenGLConfiguration()
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // This is important
		}



		void LargeVolumeRayCaster::InitGPUPageTableBuffer()
		{
			// page table
			const auto pageTableSize = pageTableManager.PageTableSize();

			const auto ptr = pageTableManager.PageTable(0).Data();

			texPageTable = OpenGLTexture::CreateTexture3D(OpenGLTexture::RGBA32UI, OpenGLTexture::Linear,
				OpenGLTexture::Linear, OpenGLTexture::ClampToEdge,
				OpenGLTexture::ClampToEdge,
				OpenGLTexture::ClampToEdge,
				OpenGLTexture::RGBAInteger,
				OpenGLTexture::UInt32, pageTableSize.x, pageTableSize.y,
				pageTableSize.z,ptr);

			// Bind to iimage3D
			texPageTable->Bind();
			texPageTable->BindToDataImage(1, 0, false, 0, OpenGLTexture::Read, OpenGLTexture::RGBA32UI);

			//const auto pageTableSize = pageTableManager.PageTableSize();
			//texPageTable->SetData(OpenGLTexture::RGBA32UI, OpenGLTexture::RGBAInteger, OpenGLTexture::UInt32,
			//	pageTableSize.x, pageTableSize.y, pageTableSize.z, pageTableManager.PageTable(0).Data());
			//texPageTable->Unbind();

		}

		void LargeVolumeRayCaster::InitGPUBlockCacheTexture()
		{
			///// GPU block Cache Texture
			const auto gpuBlockCacheSize = gpuCacheBlockSize * largeVolumeCache.BlockSize(); // number of block at every dim
			//std::cout << "GPU BLock Cache Size:" << gpuBlockCacheSize << std::endl;
			
			texCache = std::make_shared<GPUVolumeDataCache>(gpuBlockCacheSize, nullptr);
		}

		bool LargeVolumeRayCaster::CaptureAndHandleCacheFault()
		{
			const auto flag = pingpongTransferManager->TransferData(texCache.get(), &largeVolumeCache);
			
			if (!flag)
				return false;

			const auto pageTableSize = pageTableManager.PageTableSize();


			
			const auto ptr = pageTableManager.PageTable(0).Data();
			// update page table
			texPageTable->Bind();
			texPageTable->SetData(OpenGLTexture::RGBA32UI,
				OpenGLTexture::RGBAInteger,
				OpenGLTexture::UInt32,
				pageTableSize.x,
				pageTableSize.y,
				pageTableSize.z,
				ptr);
			texPageTable->Unbind();

			return true;
		}

#ifdef COUNT_VALID_BLOCK
		void LargeVolumeRayCaster::InitCounter(int capacity)
		{
			const std::size_t totalBlockCountBytes = capacity * sizeof(int);
			bufMissedHash = std::make_shared<OpenGLBuffer>(OpenGLBuffer::ShaderStorageBuffer,
				OpenGLBuffer::StreamDraw);
			bufMissedHash->AllocateFor(nullptr, totalBlockCountBytes);
			GL_ERROR_REPORT;
			bufMissedHash->BindBufferBase(2);

			atomicCounter = std::make_shared<OpenGLBuffer>(OpenGLBuffer::AtomicCounterBuffer,
				OpenGLBuffer::DynamicCopy);
			atomicCounter->AllocateFor(nullptr, sizeof(GLuint));
			atomicCounter->BindBufferBase(5);
			atomicCounter->Unbind();


		}

		int LargeVolumeRayCaster::ResetCounter()
		{
			const auto ptr = bufMissedHash->Map(OpenGLBuffer::WriteOnly);
			memset(ptr, 0, bufMissedHash->Size());
			bufMissedHash->Unmap();

			const auto counters = static_cast<int*>(atomicCounter->Map(OpenGLBuffer::ReadOnly));
			const int count = counters[0];
			atomicCounter->Unbind();

			atomicCounter->BindBufferBase(5);
			unsigned int zero = 0;
			atomicCounter->AllocateFor(&zero, sizeof(unsigned int));

			return count;

		}
#endif

		void LargeVolumeRayCaster::InitializeShaders()
		{
			// Ray casting in gpu
			rayCastingShaderProgram.create();
			rayCastingShaderProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\blockraycasting_v.glsl",
				ysl::ShaderProgram::ShaderType::Vertex);
			rayCastingShaderProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\blockraycasting_f.glsl",
				ysl::ShaderProgram::ShaderType::Fragment);
			rayCastingShaderProgram.link();

			// Generate entry and exit position
			positionShaderProgram.create();
			positionShaderProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\position_v.glsl",
				ysl::ShaderProgram::ShaderType::Vertex);
			positionShaderProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\position_f.glsl",
				ysl::ShaderProgram::ShaderType::Fragment);
			positionShaderProgram.link();

			//  Draw the result texture into the default framebuffer
			quadsShaderProgram.create();
			quadsShaderProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\quadsshader_v.glsl",
				ysl::ShaderProgram::ShaderType::Vertex);
			quadsShaderProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\quadsshader_f.glsl",
				ysl::ShaderProgram::ShaderType::Fragment);
			quadsShaderProgram.link();

			// Initialize the result texture
			clearIntermediateProgram.create();
			clearIntermediateProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\clear_v.glsl",
				ysl::ShaderProgram::ShaderType::Vertex);
			clearIntermediateProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\clear_f.glsl",
				ysl::ShaderProgram::ShaderType::Fragment);
			clearIntermediateProgram.link();
		}

		void LargeVolumeRayCaster::InitializeProxyGeometry()
		{
			//----------------------
			proxyVAO.create();
			proxyVAO.bind();
			proxyVBO = std::make_shared<OpenGLBuffer>(OpenGLBuffer::VertexArrayBuffer);
			proxyVBO->Bind();
			proxyVBO->AllocateFor(g_proxyGeometryVertices, sizeof(g_proxyGeometryVertices));

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
				reinterpret_cast<void*>(sizeof(g_proxyGeometryVertices) / 2));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));
			
			proxyEBO = std::make_shared<OpenGLBuffer>(OpenGLBuffer::ElementArrayBuffer);
			proxyEBO->Bind();
			proxyEBO->AllocateFor(g_proxyGeometryVertexIndices, sizeof(g_proxyGeometryVertexIndices));
			GL_ERROR_REPORT;
			proxyVAO.unbind();

			//----------------------------
			rayCastingVAO.create();
			rayCastingVAO.bind();

			rayCastingVBO = std::make_shared<OpenGLBuffer>(OpenGLBuffer::VertexArrayBuffer);
			rayCastingVBO->Bind();
			rayCastingVBO->AllocateFor(nullptr, 6 * sizeof(ysl::Point2f));

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<void*>(0));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<void*>(0));
			rayCastingVAO.unbind();
		}

		void LargeVolumeRayCaster::InitializeResource()
		{
			InitializeShaders();
			InitializeProxyGeometry();

			//g_pageTableX = pageTableManager.PageTableSize().x;
			//g_pageTableY = pageTableManager.PageTableSize().y;
			//g_pageTableZ = pageTableManager.PageTableSize().z;

			//g_cacheWidth = largeVolumeCache.CPUCacheSize().x;
			//g_cacheHeight = largeVolumeCache.CPUCacheSize().y;
			//g_cacheDepth = largeVolumeCache.CPUCacheSize().z;
			//int blockSize = BlockSize();
			g_repeat = largeVolumeCache.BoundaryRepeat();
			g_blockDataSize = largeVolumeCache.BlockSize();
			g_originalDataWidth = largeVolumeCache.OriginalDataSize().x;
			g_originalDataHeight = largeVolumeCache.OriginalDataSize().y;
			g_originalDataDepth = largeVolumeCache.OriginalDataSize().z;


			modelMatrix = Scale(ysl::Vector3f(g_originalDataWidth,g_originalDataHeight,g_originalDataDepth).Normalized());
		


			std::cout << "Page Table Size:" << pageTableManager.PageTableSize() << std::endl;
			std::cout << "CPU Cache Size: " << largeVolumeCache.CPUCacheSize() << std::endl;
			std::cout << "Padding: " << largeVolumeCache.BoundaryRepeat() << std::endl;
			std::cout << "Sampled data regions: " << largeVolumeCache.OriginalDataSize() << std::endl;
			std::cout << "Block Dimension: " << largeVolumeCache.SizeByBlock() << std::endl;
			std::cout << "Block Size: " << largeVolumeCache.BlockSize() << std::endl;

			OpenGLConfiguration();
			InitGPUPageTableBuffer();
			InitGPUBlockCacheTexture();
			SetShaderUniforms();
			InitTransferFunctionTexture();
			InitRayCastingTexture();



#ifdef COUNT_VALID_BLOCK
			const auto s = largeVolumeCache.SizeByBlock();
			InitCounter(s.x*s.y*s.z);
			ResetCounter();
#endif
			GL_ERROR_REPORT;
			cacheFaultHandler = std::make_shared<HashBasedGPUCacheFaultHandler>(5000, largeVolumeCache.SizeByBlock());
			GL_ERROR_REPORT;
			pingpongTransferManager = std::make_shared<PingPongTransferManager>(&pageTableManager, cacheFaultHandler.get());
			GL_ERROR_REPORT;

		}

		void LargeVolumeRayCaster::InitTransferFunctionTexture()
		{
			// Transfer Fcuntion Texture
			texTransferFunction = OpenGLTexture::CreateTexture1D(OpenGLTexture::RGBA32F,
				OpenGLTexture::Linear, OpenGLTexture::Linear,
				OpenGLTexture::ClampToEdge, OpenGLTexture::RGBA,
				OpenGLTexture::Float32, 256, nullptr);

			// Load Transfer Function
			{
				tfObject.read(tfName);
				//tfObject.read("d:\\temp.txt");
				tfObject.FetchData(tfData.data(), 256);
				texTransferFunction->SetData(OpenGLTexture::RGBA32F, OpenGLTexture::RGBA, OpenGLTexture::Float32, 256,
					0, 0, tfData.data());
			}

		}


		void LargeVolumeRayCaster::InitRayCastingTexture()
		{

			texEntryPos = OpenGLTexture::CreateTexture2DRect(OpenGLTexture::RGBA32F,
				OpenGLTexture::Linear,
				OpenGLTexture::Linear,
				OpenGLTexture::ClampToEdge,
				OpenGLTexture::ClampToEdge,
				OpenGLTexture::RGBA,
				OpenGLTexture::Float32,
				g_initialWidth,
				g_initialHeight,
				nullptr);
			texEntryPos->BindToDataImage(2, 0, false, 0, OpenGLTexture::ReadAndWrite, OpenGLTexture::RGBA32F);


			texExitPos = OpenGLTexture::CreateTexture2DRect(OpenGLTexture::RGBA32F,
				OpenGLTexture::Linear,
				OpenGLTexture::Linear,
				OpenGLTexture::ClampToEdge,
				OpenGLTexture::ClampToEdge,
				OpenGLTexture::RGBA,
				OpenGLTexture::Float32,
				g_initialWidth,
				g_initialHeight,
				nullptr);

			texIntermediateResult = OpenGLTexture::CreateTexture2DRect(OpenGLTexture::RGBA32F,
				OpenGLTexture::Linear,
				OpenGLTexture::Linear,
				OpenGLTexture::ClampToEdge,
				OpenGLTexture::ClampToEdge,
				OpenGLTexture::RGBA,
				OpenGLTexture::Float32,
				g_initialWidth,
				g_initialHeight,
				nullptr);


			texIntermediateResult->BindToDataImage(4, 0, false, 0, OpenGLTexture::ReadAndWrite,
				OpenGLTexture::RGBA32F);

			texDepth = OpenGLTexture::CreateTexture2DRect(OpenGLTexture::InternalDepthComponent,
				OpenGLTexture::Linear,
				OpenGLTexture::Linear,
				OpenGLTexture::ClampToEdge,
				OpenGLTexture::ClampToEdge,
				OpenGLTexture::ExternalDepthComponent,
				OpenGLTexture::Float32,
				g_initialWidth,
				g_initialHeight,
				nullptr);


			/// Framebuffer
			framebuffer = std::shared_ptr<OpenGLFramebufferObject>(new OpenGLFramebufferObject);
			framebuffer->Bind();
			framebuffer->AttachTexture(OpenGLFramebufferObject::ColorAttachment0, texEntryPos);
			framebuffer->AttachTexture(OpenGLFramebufferObject::ColorAttachment1, texExitPos);
			framebuffer->AttachTexture(OpenGLFramebufferObject::ColorAttachment2, texIntermediateResult);
			framebuffer->AttachTexture(OpenGLFramebufferObject::DepthAttachment, texDepth);
			framebuffer->CheckFramebufferStatus();
			framebuffer->Unbind();


		}

		void LargeVolumeRayCaster::SetShaderUniforms()
		{
			/// Set Shader Uniforms

			const auto sizeByBlock = largeVolumeCache.SizeByBlock();


			//const auto totalPageTableSize = ysl::Vector3i{ (int)sizeByBlock.x,(int)sizeByBlock.y,(int)sizeByBlock.z };
			const auto ts = pageTableManager.PageTableSize();
			const auto totalPageTableSize = ysl::Vector3i{(int)ts.x,(int)ts.y,(int)ts.z};

			const auto blockDataSize = ysl::Vector3i{g_blockDataSize - 2 * g_repeat, g_blockDataSize - 2 * g_repeat, g_blockDataSize - 2 * g_repeat};
			//const auto volumeDataSize = ysl::Vector3i{g_originalDataWidth, g_originalDataHeight, g_originalDataDepth};
			//const auto pageTableBlockEntrySize = ysl::Vector3i{ pageTableBlockEntry, pageTableBlockEntry, pageTableBlockEntry };
			const auto repeatSize = ysl::Vector3i{ g_repeat, g_repeat, g_repeat };
			const auto repeatOffset = ysl::Vector3i{ g_repeat,g_repeat,g_repeat };
			const auto blockDataSizeNoRepeat = ysl::Vector3i{ g_blockDataSize - 2 * g_repeat,g_blockDataSize - 2 * g_repeat,g_blockDataSize - 2 * g_repeat };
			const auto volumeDataSizeNoRepeat = ysl::Vector3i{ g_originalDataWidth,g_originalDataHeight,g_originalDataDepth };

			rayCastingShaderProgram.bind();
			//rayCastingShaderProgram.setUniformValue("totalPageDirSize",totalPageDirSize);
			rayCastingShaderProgram.setUniformValue("totalPageTableSize", totalPageTableSize);

			//std::cout << "SetShaderUniforms:" << totalPageTableSize << std::endl;


			rayCastingShaderProgram.setUniformValue("blockDataSize", blockDataSize);
			//rayCastingShaderProgram.setUniformValue("volumeDataSize", volumeDataSize);
			//rayCastingShaderProgram.setUniformValue("pageTableBlockEntrySize", pageTableBlockEntrySize);
			rayCastingShaderProgram.setUniformValue("repeatSize", repeatSize);
			rayCastingShaderProgram.setUniformValue("repeatOffset", repeatOffset);
			rayCastingShaderProgram.setUniformValue("blockDataSizeNoRepeat", blockDataSizeNoRepeat);
			rayCastingShaderProgram.setUniformValue("volumeDataSizeNoRepeat", volumeDataSizeNoRepeat);
			rayCastingShaderProgram.unbind();


			//std::cout << "totalPageDirSize:" << totalPageDirSize<<std::endl;
			std::cout << "totalPageTableSize:" << totalPageTableSize <<std::endl;
			std::cout << "Block Data Size:" << blockDataSize << std::endl;
			//std::cout << "Volume Data Size:" << volumeDataSize << std::endl;
			//std::cout << "Page Table Block Entry Size:" << pageTableBlockEntrySize << std::endl;
			std::cout << "Padding Size:" << repeatSize << std::endl;
			std::cout << "Repeat Offset:" << repeatOffset << std::endl;
			std::cout << "Block Data Size No Repeat:" << blockDataSizeNoRepeat << std::endl;
			std::cout << "Volume Data Size No Repeat:" << volumeDataSizeNoRepeat << std::endl;

			quadsShaderProgram.bind();
			quadsShaderProgram.unbind();
		}

	}
}

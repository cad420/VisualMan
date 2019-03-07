
#include "largevolumeraycasterapplication.h"

#include "../application/event.h"
#include "../opengl/openglutils.h"
#include "../../lib/gl3w/GL/gl3w.h"

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
			VolumeVirtualMemoryHierachy<16, 16, 16>(fileName)
		{
			camera = FocusCamera{ Point3f{0.f,0.f,5.f} };
			tfData.resize(256);
			gpuCacheBlockSize = Size3{10,10,10};
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

			g_blockBytes = BlockSize()*BlockSize()*BlockSize() * sizeof(char);


			TransferFunction::Callback cb = [this](TransferFunction * tf)
			{
				tf->FetchData(tfData.data(), 256);
				texTransferFunction->SetData(OpenGLTexture::RGBA32F,
					OpenGLTexture::RGBA,
					OpenGLTexture::Float32,
					256, 0, 0, tfData.data());
				GL_ERROR_REPORT;
			};

			TFWidget = std::make_shared<imgui::TransferFunctionWidget>("D:\\scidata\\std_tf1d.TF1D");
			TFWidget->AddUpdateCallBack(cb);

			//AddWidget(TFWidget);
		}

		void LargeVolumeRayCaster::DrawImGui()
		{
			TFWidget->Draw();

			ImGui::Begin("Control Panel");
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
			ImGui::End();


		}

		void LargeVolumeRayCaster::RenderLoop()
		{

			glClear(GL_COLOR_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			// clear intermediate result
			framebuffer->Bind();
			proxyVAO.bind();

			const auto worldMatrix = Scale(1,1,1);

			// Cull face
			positionShaderProgram.bind();
			positionShaderProgram.setUniformValue("projMatrix", projMatrix.Matrix());
			positionShaderProgram.setUniformValue("worldMatrix", worldMatrix.Matrix());
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

			do
			{
				glFinish();
				InitBlockExistsHash();
				InitMissedBlockVector();
				glDrawArrays(GL_TRIANGLES, 0, 6);
				g_renderPassPerFrame++;
			} while (CaptureAndHandleCacheMiss());



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

		void LargeVolumeRayCaster::InitBlockExistsHash()
		{
			const auto ptr = bufMissedHash->Map(OpenGLBuffer::WriteOnly);
			memset(ptr, 0, bufMissedHash->Size());
			bufMissedHash->Unmap();
			GL_ERROR_REPORT;
		}

		void LargeVolumeRayCaster::InitMissedBlockVector()
		{
			const auto ptr = bufMissedTable->Map(OpenGLBuffer::WriteOnly);
			memset(ptr, 0, bufMissedTable->Size());
			bufMissedTable->Unmap();
			atomicCounter->BindBufferBase(3);
			unsigned int zero = 0;
			atomicCounter->AllocateFor(&zero, sizeof(unsigned int));
			GL_ERROR_REPORT;
		}

		void LargeVolumeRayCaster::InitGPUPageTableBuffer()
		{
			// page table
			texPageTable = OpenGLTexture::CreateTexture3D(OpenGLTexture::RGBA32UI, OpenGLTexture::Linear,
				OpenGLTexture::Linear, OpenGLTexture::ClampToEdge,
				OpenGLTexture::ClampToEdge,
				OpenGLTexture::ClampToEdge,
				OpenGLTexture::RGBAInteger,
				OpenGLTexture::UInt32, g_pageTableX, g_pageTableY,
				g_pageTableZ, PageTable->Data());
			// Bind to iimage3D
			texPageTable->Bind();
			texPageTable->BindToDataImage(1, 0, false, 0, OpenGLTexture::Read, OpenGLTexture::RGBA32UI);

		}

		void LargeVolumeRayCaster::InitGPUBlockCacheTexture()
		{
			///// GPU block Cache Texture
			ysl::Size3 gpuBlockCacheSize = gpuCacheBlockSize * BlockSize(); // number of block at every dim
			texCache = OpenGLTexture::CreateTexture3D(OpenGLTexture::R8, // R16F
				OpenGLTexture::Linear,
				OpenGLTexture::Linear,
				OpenGLTexture::ClampToEdge,
				OpenGLTexture::ClampToEdge,
				OpenGLTexture::ClampToEdge,
				OpenGLTexture::RED,
				OpenGLTexture::UInt8,
				gpuBlockCacheSize.x,
				gpuBlockCacheSize.y,
				gpuBlockCacheSize.z, nullptr);

		}

		void LargeVolumeRayCaster::initGPUCacheLRUPolicyList()
		{
			const auto size = BlockSize();
			//const auto w = xCacheBlockCount(), h = yCacheBlockCount(), d = zCacheBlockCount();
			for (auto z = 0; z < gpuCacheBlockSize.x; z++)
				for (auto y = 0; y < gpuCacheBlockSize.y; y++)
					for (auto x = 0; x < gpuCacheBlockSize.z; x++)
					{
						g_lruList.push_back(std::make_pair(PageTableEntryAbstractIndex(-1, -1, -1),
							CacheBlockAbstractIndex(x * size, y * size, z * size)));
					}

			texPageTable->SetData(OpenGLTexture::RGBA32UI, OpenGLTexture::RGBAInteger, OpenGLTexture::UInt32,
				g_pageTableX, g_pageTableY, g_pageTableZ, PageTable->Data());
		}

		void LargeVolumeRayCaster::InitPingPongSwapPBO()
		{
			//// Ping-Pong Buffer 
			blockPingBuf = std::make_shared<OpenGLBuffer>(OpenGLBuffer::PixelUnpackBuffer, OpenGLBuffer::StreamDraw);
			blockPingBuf->AllocateFor(nullptr, g_blockDataSize * g_blockDataSize * g_blockDataSize * sizeof(char));
			blockPingBuf->Unbind();
			blockPongBuf = std::make_shared<OpenGLBuffer>(OpenGLBuffer::PixelUnpackBuffer, OpenGLBuffer::StreamDraw);
			blockPongBuf->AllocateFor(nullptr, g_blockDataSize * g_blockDataSize * g_blockDataSize * sizeof(char));
			blockPongBuf->Unbind();
			GL_ERROR_REPORT;
		}

		void LargeVolumeRayCaster::InitHashBuffer()
		{
			/// GPU Hash Buffer

			const std::size_t totalBlockCountBytes = std::size_t(SizeByBlock().x) * std::size_t(SizeByBlock().y) * std::
				size_t(SizeByBlock().z) * sizeof(int);
			bufMissedHash = std::make_shared<OpenGLBuffer>(OpenGLBuffer::ShaderStorageBuffer,
				OpenGLBuffer::StreamDraw);
			bufMissedHash->AllocateFor(nullptr, totalBlockCountBytes);
			GL_ERROR_REPORT;
			bufMissedHash->BindBufferBase(0);
		}
		void LargeVolumeRayCaster::InitMissTableBuffer()
		{
			/// GPU Missed Table Buffer

			const auto missedBlockCapacity = 5000 * sizeof(unsigned int);
			bufMissedTable = std::make_shared<OpenGLBuffer>(OpenGLBuffer::ShaderStorageBuffer,
				OpenGLBuffer::StreamDraw);
			bufMissedTable->AllocateFor(nullptr, missedBlockCapacity);
			GL_ERROR_REPORT;
			bufMissedTable->BindBufferBase(1);
			//g_cacheMissTablePtr = (int*)g_bufMissedTable->Map(OpenGLBuffer::WriteOnly);

		}
		void LargeVolumeRayCaster::InitGPUAtomicCounter()
		{
			/// GPU Atomic Counter
			atomicCounter = std::make_shared<OpenGLBuffer>(OpenGLBuffer::AtomicCounterBuffer,
				OpenGLBuffer::DynamicCopy);
			atomicCounter->AllocateFor(nullptr, sizeof(GLuint));
			atomicCounter->BindBufferBase(3);
			atomicCounter->Unbind();
			GL_ERROR_REPORT;
		}



		bool LargeVolumeRayCaster::CaptureAndHandleCacheMiss()
		{
			// There is a run-time error at nvoglv64.dll
			const auto counters = static_cast<int*>(atomicCounter->Map(OpenGLBuffer::ReadOnly));
			const int count = counters[0];
			if (count == 0)
				return false;

			const std::size_t cacheBlockThreshold = gpuCacheBlockSize.x * gpuCacheBlockSize.y * gpuCacheBlockSize.
				z;
			const std::size_t blockSize = BlockSize();
			const auto missedBlocks = (std::min)(count, (int)cacheBlockThreshold);


			hits.clear();
			posInCache.clear();
			hits.reserve(missedBlocks);
			posInCache.reserve(missedBlocks * 3);

			const auto ptr = static_cast<int*>(bufMissedTable->Map(OpenGLBuffer::ReadWrite));
			for (auto i = 0; i < missedBlocks; i++)
			{
				const auto blockId = ptr[i];
				hits.emplace_back(blockId, g_xBlockCount, g_yBlockCount, g_zBlockCount);
			}
			bufMissedTable->Unmap();


			auto& pageTable = *PageTable;
			auto& LRUList = g_lruList;

			// Update LRU List 
			for (int i = 0; i < missedBlocks; i++)
			{
				const auto& index = hits[i];
				auto& pageTableEntry = pageTable(index.x, index.y, index.z);
				auto& last = LRUList.back();
				pageTableEntry.w = EntryFlag::Mapped; // Map the flag of page table entry
				// last.second is the cache block index

				const auto xInCache = last.second.x;
				const auto yInCache = last.second.y;
				const auto zInCache = last.second.z;

				pageTableEntry.x = xInCache; // fill the page table entry
				pageTableEntry.y = yInCache;
				pageTableEntry.z = zInCache;

				if (last.first.x != -1)
				{
					pageTable(last.first.x, last.first.y, last.first.z).w = EntryFlag::Unmapped;
				}

				last.first.x = index.x;
				last.first.y = index.y;
				last.first.z = index.z;
				LRUList.splice(LRUList.begin(), LRUList, --LRUList.end()); // move from tail to head, LRU policy

				posInCache.push_back(xInCache);
				posInCache.push_back(yInCache);
				posInCache.push_back(zInCache);
			}
			// Update load missed block  data to GPU

			const auto blockBytes = blockSize * blockSize * blockSize * sizeof(char);

			// initialize second pbo 
			// Ping-Pong PBO Transfer

			long long accessPageTableTime = 0,
				readDataTime = 0,
				copyDataTime = 0,
				dmaTime = 0,
				totalTime = 0,
				bindTime = 0;


			std::shared_ptr<OpenGLBuffer> pbo[2] = { blockPingBuf, blockPongBuf };
			auto curPBO = 0;
			auto i = 0;
			const auto& idx = hits[i];
			const auto dd = ReadBlockDataFromCache(idx);

			pbo[1 - curPBO]->Bind();
			auto pp = pbo[1 - curPBO]->Map(OpenGLBuffer::WriteOnly);
			memcpy(pp, dd, blockBytes);
			pbo[1 - curPBO]->Unmap(); // copy data to pbo
			pbo[1 - curPBO]->Bind();

			texCache->Bind();

			g_timer.begin();
			for (; i < missedBlocks - 1;)
			{
				pbo[1 - curPBO]->Bind();
				texCache->SetSubData(OpenGLTexture::RED,
					OpenGLTexture::UInt8,
					posInCache[3 * i], blockSize,
					posInCache[3 * i + 1], blockSize,
					posInCache[3 * i + 2], blockSize,
					nullptr);
				pbo[1 - curPBO]->Unbind();
				i++;
				const auto& index = hits[i];
				const auto d = ReadBlockDataFromCache(index);
				pbo[curPBO]->Bind();
				auto p = pbo[curPBO]->Map(OpenGLBuffer::WriteOnly);
				memcpy(p, d, blockBytes);
				pbo[curPBO]->Unmap(); // copy data to pbo
				curPBO = 1 - curPBO;
			}
			pbo[1 - curPBO]->Bind();
			texCache->SetSubData(OpenGLTexture::RED,
				OpenGLTexture::UInt8,
				posInCache[3 * i], blockSize,
				posInCache[3 * i + 1], blockSize,
				posInCache[3 * i + 2], blockSize,
				nullptr);

			pbo[1 - curPBO]->Unbind();
			g_timer.end();

			g_missingCacheCountPerFrame += missedBlocks;
			g_uploadBlockCountPerFrame += missedBlocks;
			g_blockUploadMicroSecondsPerFrame += g_timer.duration();

			//ysl::Log("%d %d %d\n", missedBlocks, g_timer.duration(), g_timer.duration() / missedBlocks);

			//////////////////
			// update page table
			texPageTable->Bind();
			texPageTable->SetData(OpenGLTexture::RGBA32UI,
				OpenGLTexture::RGBAInteger,
				OpenGLTexture::UInt32,
				g_pageTableX,
				g_pageTableY,
				g_pageTableZ,
				pageTable.Data());
			texPageTable->Unbind();

			return true;
		}

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

			g_pageTableX = PageTable->Size().x;
			g_pageTableY = PageTable->Size().y;
			g_pageTableZ = PageTable->Size().z;

			g_cacheWidth = CacheSize().x;
			g_cacheHeight = CacheSize().y;
			g_cacheDepth = CacheSize().z;
			//int blockSize = BlockSize();
			g_repeat = BoundaryRepeat();
			g_blockDataSize = BlockSize();
			g_originalDataWidth = OriginalDataSize().x;
			g_originalDataHeight = OriginalDataSize().y;
			g_originalDataDepth = OriginalDataSize().z;
			g_xBlockCount = SizeByBlock().x;
			g_yBlockCount = SizeByBlock().y;
			g_zBlockCount = SizeByBlock().z;

			//std::cout << "pageDirX:" << g_pageDirX << std::endl;
			//std::cout << "pageDirY:" << g_pageDirY << std::endl;
			//std::cout << "pageDirZ:" << g_pageDirZ << std::endl;

			std::cout << "pageTableX:" << g_pageTableX << std::endl;
			std::cout << "pageTableY:" << g_pageTableY << std::endl;
			std::cout << "pageTableZ:" << g_pageTableZ << std::endl;
			std::cout << "cacheWidth:" << g_cacheWidth << std::endl;
			std::cout << "cacheHeight:" << g_cacheHeight << std::endl;
			std::cout << "cacheDepth:" << g_cacheDepth << std::endl;
			std::cout << "repeat:" << g_repeat << std::endl;
			std::cout << "original data x:" << g_originalDataWidth << std::endl;
			std::cout << "original data y:" << g_originalDataHeight << std::endl;
			std::cout << "original data z:" << g_originalDataDepth << std::endl;
			std::cout << "BlockSize:" << BlockSize() << std::endl;

			OpenGLConfiguration();
			InitGPUPageTableBuffer();
			initGPUCacheLRUPolicyList();
			InitGPUBlockCacheTexture();
			InitPingPongSwapPBO();
			InitHashBuffer();
			InitMissTableBuffer();
			InitGPUAtomicCounter();
			SetShaderUniforms();
			InitTransferFunctionTexture();
			InitRayCastingTexture();
			InitBlockExistsHash();
			InitMissedBlockVector();
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
				tfObject.read("d:\\scidata\\default.tf1d");
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
			const int pageDirX = PageDir->Size().x;
			const int pageDirY = PageDir->Size().y;
			const int pageDirZ = PageDir->Size().z;
			rayCastingShaderProgram.bind();

			rayCastingShaderProgram.setUniformValue("totalPageDirSize",
				ysl::Vector3i{ pageDirX, pageDirY, pageDirZ });
			rayCastingShaderProgram.setUniformValue("totalPageTableSize",
				ysl::Vector3i{ g_pageTableX, g_pageTableY, g_pageTableZ });
			rayCastingShaderProgram.setUniformValue("blockDataSize", ysl::Vector3i{
				g_blockDataSize - 2 * g_repeat, g_blockDataSize - 2 * g_repeat, g_blockDataSize - 2 * g_repeat
				});
			rayCastingShaderProgram.setUniformValue("volumeDataSize",
				ysl::Vector3i{
					g_originalDataWidth, g_originalDataDepth, g_originalDataDepth
				});
			rayCastingShaderProgram.setUniformValue("pageTableBlockEntrySize",
				ysl::Vector3i{
					pageTableBlockEntry, pageTableBlockEntry, pageTableBlockEntry
				});
			rayCastingShaderProgram.setUniformValue("repeatSize", ysl::Vector3i{ g_repeat, g_repeat, g_repeat });

			rayCastingShaderProgram.setUniformValue("repeatOffset", ysl::Vector3i{ g_repeat,g_repeat,g_repeat });


			rayCastingShaderProgram.setUniformValue("totalPageTableSize", ysl::Vector3i{ g_pageTableX,g_pageTableY,g_pageTableZ });
			rayCastingShaderProgram.setUniformValue("blockDataSizeNoRepeat", ysl::Vector3i{ g_blockDataSize - 2 * g_repeat,g_blockDataSize - 2 * g_repeat,g_blockDataSize - 2 * g_repeat });
			rayCastingShaderProgram.setUniformValue("volumeDataSizeNoRepeat", ysl::Vector3i{ g_originalDataWidth,g_originalDataDepth,g_originalDataDepth });
			rayCastingShaderProgram.setUniformValue("pageTableBlockEntrySize", ysl::Vector3i{ pageTableBlockEntry ,pageTableBlockEntry ,pageTableBlockEntry });
			rayCastingShaderProgram.unbind();


			quadsShaderProgram.bind();
			quadsShaderProgram.unbind();
		}

	}
}

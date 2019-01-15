
#include "largevolumerenderer.h"

namespace ysl
{
	namespace app
	{

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


		void LargeVolumeRenderApplication::InitBlockExistsHash()
		{
			const auto ptr = g_bufMissedHash->Map(OpenGLBuffer::WriteOnly);
			memset(ptr, 0, g_bufMissedHash->Size());
			g_bufMissedHash->Unmap();
			GL_ERROR_REPORT;
		}

		void LargeVolumeRenderApplication::InitMissedBlockVector()
		{
			const auto ptr = g_bufMissedTable->Map(OpenGLBuffer::WriteOnly);
			memset(ptr, 0, g_bufMissedTable->Size());
			g_bufMissedTable->Unmap();
			g_atomicCounter->BindBufferBase(3);
			unsigned int zero = 0;
			g_atomicCounter->AllocateFor(&zero, sizeof(unsigned int));
			GL_ERROR_REPORT;
		}

		void LargeVolumeRenderApplication::InitGPUCacheLRUPolicyList()
		{
			const auto size = g_largeVolumeData->BlockSize();
			//const auto w = xCacheBlockCount(), h = yCacheBlockCount(), d = zCacheBlockCount();
			for (auto z = 0; z < g_gpuCacheBlockSize.x; z++)
				for (auto y = 0; y < g_gpuCacheBlockSize.y; y++)
					for (auto x = 0; x < g_gpuCacheBlockSize.z; x++)
					{
						g_lruList.push_back(std::make_pair(PageTableEntryAbstractIndex(-1, -1, -1),
						                                   CacheBlockAbstractIndex(x * size, y * size, z * size)));
					}

			g_texPageTable->SetData(OpenGLTexture::RGBA32UI, OpenGLTexture::RGBAInteger, OpenGLTexture::UInt32,
			                        g_pageTableX, g_pageTableY, g_pageTableZ, g_largeVolumeData->PageTable->Data());
		}

		bool LargeVolumeRenderApplication::CaptureAndHandleCacheMiss()
		{
			// There is a run-time error at nvoglv64.dll
			const auto counters = static_cast<int*>(g_atomicCounter->Map(OpenGLBuffer::ReadOnly));
			const int count = counters[0];
			if (count == 0)
				return false;

			const std::size_t cacheBlockThreshold = g_gpuCacheBlockSize.x * g_gpuCacheBlockSize.y * g_gpuCacheBlockSize.
				z;
			const std::size_t blockSize = g_largeVolumeData->BlockSize();
			const auto missedBlocks = (std::min)(count, (int)cacheBlockThreshold);


			g_hits.clear();
			g_posInCache.clear();
			g_hits.reserve(missedBlocks);
			g_posInCache.reserve(missedBlocks * 3);

			const auto ptr = static_cast<int*>(g_bufMissedTable->Map(OpenGLBuffer::ReadWrite));
			for (auto i = 0; i < missedBlocks; i++)
			{
				const auto blockId = ptr[i];
				g_hits.emplace_back(blockId, g_xBlockCount, g_yBlockCount, g_zBlockCount);
			}
			g_bufMissedTable->Unmap();


			auto& pageTable = *g_largeVolumeData->PageTable;
			auto& LRUList = g_lruList;

			// Update LRU List 
			for (int i = 0; i < missedBlocks; i++)
			{
				const auto& index = g_hits[i];
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

				g_posInCache.push_back(xInCache);
				g_posInCache.push_back(yInCache);
				g_posInCache.push_back(zInCache);
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


			std::shared_ptr<OpenGLBuffer> pbo[2] = {g_blockPingBuf, g_blockPongBuf};
			auto curPBO = 0;
			auto i = 0;
			const auto& idx = g_hits[i];
			const auto dd = g_largeVolumeData->ReadBlockDataFromCache(idx);

			pbo[1 - curPBO]->Bind();
			auto pp = pbo[1 - curPBO]->Map(OpenGLBuffer::WriteOnly);
			memcpy(pp, dd, blockBytes);
			pbo[1 - curPBO]->Unmap(); // copy data to pbo
			pbo[1 - curPBO]->Bind();

			g_texCache->Bind();

			g_timer.begin();
			for (; i < missedBlocks - 1;)
			{
				pbo[1 - curPBO]->Bind();
				g_texCache->SetSubData(OpenGLTexture::RED,
				                       OpenGLTexture::UInt8,
				                       g_posInCache[3 * i], blockSize,
				                       g_posInCache[3 * i + 1], blockSize,
				                       g_posInCache[3 * i + 2], blockSize,
				                       nullptr);
				pbo[1 - curPBO]->Unbind();
				i++;
				const auto& index = g_hits[i];
				const auto d = g_largeVolumeData->ReadBlockDataFromCache(index);
				pbo[curPBO]->Bind();
				auto p = pbo[curPBO]->Map(OpenGLBuffer::WriteOnly);
				memcpy(p, d, blockBytes);
				pbo[curPBO]->Unmap(); // copy data to pbo
				curPBO = 1 - curPBO;
			}
			pbo[1 - curPBO]->Bind();
			g_texCache->SetSubData(OpenGLTexture::RED,
			                       OpenGLTexture::UInt8,
			                       g_posInCache[3 * i], blockSize,
			                       g_posInCache[3 * i + 1], blockSize,
			                       g_posInCache[3 * i + 2], blockSize,
			                       nullptr);

			pbo[1 - curPBO]->Unbind();
			g_timer.end();

			g_missingCacheCountPerFrame += missedBlocks;
			g_uploadBlockCountPerFrame += missedBlocks;
			g_blockUploadMicroSecondsPerFrame += g_timer.duration();

			//ysl::Log("%d %d %d\n", missedBlocks, g_timer.duration(), g_timer.duration() / missedBlocks);

			//////////////////


			//g_timer.begin();
			//g_texCache->Bind();
			//unsigned int pbo[2] = { g_pingPBO,g_pongPBO };
			//auto curPBO = 0;
			//auto i = 0;
			//const auto & idx = g_hits[i];
			//const auto dd = g_largeVolumeData->ReadBlockDataFromCache(idx);
			////glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo[1 - curPBO]);
			//memcpy(g_pboPtr[1-curPBO], dd, blockBytes);
			//for (; i < missedBlocks - 1;)
			//{
			//	//using namespace std::chrono_literals;
			//	//std::this_thread::sleep_for(50ms);
			//	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo[1 - curPBO]);
			//	GL_ERROR_REPORT;
			//	g_texCache->SetSubData(OpenGLTexture::RED,
			//		OpenGLTexture::UInt8,
			//		g_posInCache[3 * i], blockSize,
			//		g_posInCache[3 * i + 1], blockSize,
			//		g_posInCache[3 * i + 2], blockSize,
			//		nullptr);
			//	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

			//	//ysl::Log("pos in cache:[%d, %d, %d], write to %u\n", 
			//	//	g_posInCache[3 * i],
			//	//	g_posInCache[3 * i + 1],
			//	//	g_posInCache[3 * i + 2],g_pboPtr[1-curPBO]);

			//	GLsync s = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
			//	glClientWaitSync(s, 0, 1000000);
			//	glDeleteSync(s);

			//	i++;
			//	const auto & index = g_hits[i];
			//	const auto d = g_largeVolumeData->ReadBlockDataFromCache(index);
			//	memcpy(g_pboPtr[curPBO], d, blockBytes);
			//	curPBO = 1 - curPBO;

			//}

			//glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo[1 - curPBO]);
			//g_texCache->SetSubData(OpenGLTexture::RED,
			//	OpenGLTexture::UInt8,
			//	g_posInCache[3 * i], blockSize,
			//	g_posInCache[3 * i + 1], blockSize,
			//	g_posInCache[3 * i + 2], blockSize,
			//	nullptr);
			//glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
			//g_texCache->Unbind();

			//g_timer.end();
			//ysl::Log("%d %d %d\n", missedBlocks, g_timer.duration(), g_timer.duration() / missedBlocks);


			///////

			//g_texCache->Bind();
			//glBindBuffer(GL_PIXEL_UNPACK_BUFFER, g_pingPBO);
			//g_timer.begin();
			////GL_ERROR_REPORT;
			//for(int i = 0;i<missedBlocks;i++)
			//{
			//	const auto & index = g_hits[i];
			//	//const auto d = g_largeVolumeData->ReadBlockDataFromCache(index);
			//	memcpy(g_pboPtr[0], g_data, blockBytes);

			//	g_texCache->SetSubData(OpenGLTexture::RED,
			//		OpenGLTexture::UInt8,
			//		g_posInCache[3 * i], blockSize,
			//		g_posInCache[3 * i + 1], blockSize,
			//		g_posInCache[3 * i + 2], blockSize,
			//		nullptr);

			//	GLsync s = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
			//	glClientWaitSync(s, 0, 1000000);
			//	glDeleteSync(s);

			//}
			//glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
			//g_texCache->Unbind();

			//ysl::Log("%d %d %d\n", missedBlocks, g_timer.duration(), g_timer.duration() / missedBlocks);
			//ysl::Log("blockCount:%d\nreadDataTime:%d\ncopyDataTime:%d\nDMATime:%d\ntotalTime:%d\nbindTime:%d\n", missedBlocks,readDataTime,copyDataTime,dmaTime,totalTime,bindTime);

			// update page table
			g_texPageTable->Bind();
			g_texPageTable->SetData(OpenGLTexture::RGBA32UI,
			                        OpenGLTexture::RGBAInteger,
			                        OpenGLTexture::UInt32,
			                        g_pageTableX,
			                        g_pageTableY,
			                        g_pageTableZ,
			                        pageTable.Data());
			g_texPageTable->Unbind();

			return true;
		}

		void LargeVolumeRenderApplication::InitializeShaders()
		{
			// Ray casting in gpu
			g_rayCastingShaderProgram.create();
			g_rayCastingShaderProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\blockraycasting_v.glsl",
			                                            ysl::ShaderProgram::ShaderType::Vertex);
			g_rayCastingShaderProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\blockraycasting_f.glsl",
			                                            ysl::ShaderProgram::ShaderType::Fragment);
			g_rayCastingShaderProgram.link();

			// Generate entry and exit position
			g_positionShaderProgram.create();
			g_positionShaderProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\position_v.glsl",
			                                          ysl::ShaderProgram::ShaderType::Vertex);
			g_positionShaderProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\position_f.glsl",
			                                          ysl::ShaderProgram::ShaderType::Fragment);
			g_positionShaderProgram.link();

			//  Draw the result texture into the default framebuffer
			g_quadsShaderProgram.create();
			g_quadsShaderProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\quadsshader_v.glsl",
			                                       ysl::ShaderProgram::ShaderType::Vertex);
			g_quadsShaderProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\quadsshader_f.glsl",
			                                       ysl::ShaderProgram::ShaderType::Fragment);
			g_quadsShaderProgram.link();

			// Initialize the result texture
			g_clearIntermediateProgram.create();
			g_clearIntermediateProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\clear_v.glsl",
			                                             ysl::ShaderProgram::ShaderType::Vertex);
			g_clearIntermediateProgram.addShaderFromFile("D:\\code\\MRE\\src\\shader\\clear_f.glsl",
			                                             ysl::ShaderProgram::ShaderType::Fragment);
			g_clearIntermediateProgram.link();
		}

		void LargeVolumeRenderApplication::InitializeProxyGeometry()
		{
			GL_ERROR_REPORT;

			g_proxyVAO.create();
			g_proxyVAO.bind();

			g_proxyVBO = std::make_shared<OpenGLBuffer>(OpenGLBuffer::VertexArrayBuffer);
			g_proxyVBO->Bind();
			g_proxyVBO->AllocateFor(g_proxyGeometryVertices, sizeof(g_proxyGeometryVertices));

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
			                      reinterpret_cast<void*>(sizeof(g_proxyGeometryVertices) / 2));
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
		}

		void LargeVolumeRenderApplication::InitializeResource()
		{
			InitializeShaders();
			InitializeProxyGeometry();

			g_largeVolumeData.reset(
				new VolumeVirtualMemoryHierachy<pageTableBlockEntry, pageTableBlockEntry, pageTableBlockEntry>(
					g_lvdFileName));

			g_pageDirX = g_largeVolumeData->PageDir->Size().x;
			g_pageDirY = g_largeVolumeData->PageDir->Size().y;
			g_pageDirZ = g_largeVolumeData->PageDir->Size().z;

			g_pageTableX = g_largeVolumeData->PageTable->Size().x;
			g_pageTableY = g_largeVolumeData->PageTable->Size().y;
			g_pageTableZ = g_largeVolumeData->PageTable->Size().z;

			g_cacheWidth = g_largeVolumeData->CacheSize().x;
			g_cacheHeight = g_largeVolumeData->CacheSize().y;
			g_cacheDepth = g_largeVolumeData->CacheSize().z;

			int blockSize = g_largeVolumeData->BlockSize();
			ysl::Size3 gpuBlockCacheSize = g_gpuCacheBlockSize * blockSize; // number of block at every dim


			g_repeat = g_largeVolumeData->BoundaryRepeat();
			g_blockDataSize = g_largeVolumeData->BlockSize();
			g_originalDataWidth = g_largeVolumeData->OriginalDataSize().x;
			g_originalDataHeight = g_largeVolumeData->OriginalDataSize().y;
			g_originalDataDepth = g_largeVolumeData->OriginalDataSize().z;
			g_xBlockCount = g_largeVolumeData->SizeByBlock().x;
			g_yBlockCount = g_largeVolumeData->SizeByBlock().y;
			g_zBlockCount = g_largeVolumeData->SizeByBlock().z;

			const std::size_t totalBlockCountBytes = std::size_t(g_xBlockCount) * std::size_t(g_yBlockCount) * std::
				size_t(g_zBlockCount) * sizeof(int);

			std::cout << "pageDirX:" << g_pageDirX << std::endl;
			std::cout << "pageDirY:" << g_pageDirY << std::endl;
			std::cout << "pageDirZ:" << g_pageDirZ << std::endl;
			std::cout << "pageTableX:" << g_pageTableX << std::endl;
			std::cout << "pageTableY:" << g_pageTableY << std::endl;
			std::cout << "pageTableZ:" << g_pageTableZ << std::endl;
			std::cout << "cacheWidth:" << g_cacheWidth << std::endl;
			std::cout << "cacheHeight:" << g_cacheHeight << std::endl;
			std::cout << "cacheDepth:" << g_cacheDepth << std::endl;
			std::cout << "repeat:" << g_repeat << std::endl;
			//std::cout << "data size:" << g_largeVolumeData->Width() << std::endl;
			std::cout << "original data x:" << g_originalDataWidth << std::endl;
			std::cout << "original data y:" << g_originalDataHeight << std::endl;
			std::cout << "original data z:" << g_originalDataDepth << std::endl;
			std::cout << "BlockSize:" << g_largeVolumeData->BlockSize() << std::endl;

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // This is important

			GL_ERROR_REPORT;
			// page table
			g_texPageTable = OpenGLTexture::CreateTexture3D(OpenGLTexture::RGBA32UI, OpenGLTexture::Linear,
			                                                OpenGLTexture::Linear, OpenGLTexture::ClampToEdge,
			                                                OpenGLTexture::ClampToEdge,
			                                                OpenGLTexture::ClampToEdge,
			                                                OpenGLTexture::RGBAInteger,
			                                                OpenGLTexture::UInt32, g_pageTableX, g_pageTableY,
			                                                g_pageTableZ, g_largeVolumeData->PageTable->Data());
			// Bind to iimage3D
			g_texPageTable->Bind();
			g_texPageTable->BindToDataImage(1, 0, false, 0, OpenGLTexture::Read, OpenGLTexture::RGBA32UI);

			// allocate for volume data block cache


			InitGPUCacheLRUPolicyList();
			g_texCache = OpenGLTexture::CreateTexture3D(OpenGLTexture::R8, // R16F
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
			g_blockPingBuf = std::make_shared<OpenGLBuffer>(OpenGLBuffer::PixelUnpackBuffer, OpenGLBuffer::StreamDraw);
			g_blockPingBuf->AllocateFor(nullptr, g_blockDataSize * g_blockDataSize * g_blockDataSize * sizeof(char));
			g_blockPingBuf->Unbind();
			g_blockPongBuf = std::make_shared<OpenGLBuffer>(OpenGLBuffer::PixelUnpackBuffer, OpenGLBuffer::StreamDraw);
			g_blockPongBuf->AllocateFor(nullptr, g_blockDataSize * g_blockDataSize * g_blockDataSize * sizeof(char));
			g_blockPongBuf->Unbind();
			GL_ERROR_REPORT;

			g_bufMissedHash = std::make_shared<OpenGLBuffer>(OpenGLBuffer::ShaderStorageBuffer,
			                                                 OpenGLBuffer::StreamDraw);
			g_bufMissedHash->AllocateFor(nullptr, totalBlockCountBytes);
			GL_ERROR_REPORT;
			g_bufMissedHash->BindBufferBase(0);
			//g_cacheHashPtr = (int*)g_bufMissedHash->Map(OpenGLBuffer::WriteOnly);

			g_bufMissedTable = std::make_shared<OpenGLBuffer>(OpenGLBuffer::ShaderStorageBuffer,
			                                                  OpenGLBuffer::StreamDraw);
			g_bufMissedTable->AllocateFor(nullptr, missedBlockCapacity);
			GL_ERROR_REPORT;
			g_bufMissedTable->BindBufferBase(1);
			//g_cacheMissTablePtr = (int*)g_bufMissedTable->Map(OpenGLBuffer::WriteOnly);

			g_atomicCounter = std::make_shared<OpenGLBuffer>(OpenGLBuffer::AtomicCounterBuffer,
			                                                 OpenGLBuffer::DynamicCopy);
			g_atomicCounter->AllocateFor(nullptr, sizeof(GLuint));
			g_atomicCounter->BindBufferBase(3);
			g_atomicCounter->Unbind();
			GL_ERROR_REPORT;


			// shader program
			g_rayCastingShaderProgram.bind();
			//g_rayCastingShaderProgram.setUniformSampler("cacheVolume", OpenGLTexture::TextureUnit4, *g_texCache);
			g_rayCastingShaderProgram.setUniformValue("totalPageDirSize",
			                                          ysl::Vector3i{g_pageDirX, g_pageDirY, g_pageDirZ});
			g_rayCastingShaderProgram.setUniformValue("totalPageTableSize",
			                                          ysl::Vector3i{g_pageTableX, g_pageTableY, g_pageTableZ});
			g_rayCastingShaderProgram.setUniformValue("blockDataSize", ysl::Vector3i{
				g_blockDataSize - 2 * g_repeat, g_blockDataSize - 2 * g_repeat, g_blockDataSize - 2 * g_repeat
			});
			g_rayCastingShaderProgram.setUniformValue("volumeDataSize",
			                                          ysl::Vector3i{
				                                          g_originalDataWidth, g_originalDataDepth, g_originalDataDepth
			                                          });
			g_rayCastingShaderProgram.setUniformValue("pageTableBlockEntrySize",
			                                          ysl::Vector3i{
				                                          pageTableBlockEntry, pageTableBlockEntry, pageTableBlockEntry
			                                          });
			g_rayCastingShaderProgram.setUniformValue("repeatSize", ysl::Vector3i{g_repeat, g_repeat, g_repeat});
			g_rayCastingShaderProgram.unbind();

			g_texTransferFunction = OpenGLTexture::CreateTexture1D(OpenGLTexture::RGBA32F,
			                                                       OpenGLTexture::Linear, OpenGLTexture::Linear,
			                                                       OpenGLTexture::ClampToEdge, OpenGLTexture::RGBA,
			                                                       OpenGLTexture::Float32, 256, nullptr);

			g_texEntryPos = OpenGLTexture::CreateTexture2DRect(OpenGLTexture::RGBA32F,
			                                                   OpenGLTexture::Linear,
			                                                   OpenGLTexture::Linear,
			                                                   OpenGLTexture::ClampToEdge,
			                                                   OpenGLTexture::ClampToEdge,
			                                                   OpenGLTexture::RGBA,
			                                                   OpenGLTexture::Float32,
			                                                   g_initialWidth,
			                                                   g_initialHeight,
			                                                   nullptr);
			g_texEntryPos->BindToDataImage(2, 0, false, 0, OpenGLTexture::ReadAndWrite, OpenGLTexture::RGBA32F);


			g_texExitPos = OpenGLTexture::CreateTexture2DRect(OpenGLTexture::RGBA32F,
			                                                  OpenGLTexture::Linear,
			                                                  OpenGLTexture::Linear,
			                                                  OpenGLTexture::ClampToEdge,
			                                                  OpenGLTexture::ClampToEdge,
			                                                  OpenGLTexture::RGBA,
			                                                  OpenGLTexture::Float32,
			                                                  g_initialWidth,
			                                                  g_initialHeight,
			                                                  nullptr);

			g_texIntermediateResult = OpenGLTexture::CreateTexture2DRect(OpenGLTexture::RGBA32F,
			                                                             OpenGLTexture::Linear,
			                                                             OpenGLTexture::Linear,
			                                                             OpenGLTexture::ClampToEdge,
			                                                             OpenGLTexture::ClampToEdge,
			                                                             OpenGLTexture::RGBA,
			                                                             OpenGLTexture::Float32,
			                                                             g_initialWidth,
			                                                             g_initialHeight,
			                                                             nullptr);
			g_texIntermediateResult->BindToDataImage(4, 0, false, 0, OpenGLTexture::ReadAndWrite,
			                                         OpenGLTexture::RGBA32F);

			g_texDepth = OpenGLTexture::CreateTexture2DRect(OpenGLTexture::InternalDepthComponent,
			                                                OpenGLTexture::Linear,
			                                                OpenGLTexture::Linear,
			                                                OpenGLTexture::ClampToEdge,
			                                                OpenGLTexture::ClampToEdge,
			                                                OpenGLTexture::ExternalDepthComponent,
			                                                OpenGLTexture::Float32,
			                                                g_initialWidth,
			                                                g_initialHeight,
			                                                nullptr);

			g_framebuffer = std::shared_ptr<OpenGLFramebufferObject>(new OpenGLFramebufferObject);
			g_framebuffer->Bind();
			g_framebuffer->AttachTexture(OpenGLFramebufferObject::ColorAttachment0, g_texEntryPos);
			g_framebuffer->AttachTexture(OpenGLFramebufferObject::ColorAttachment1, g_texExitPos);
			g_framebuffer->AttachTexture(OpenGLFramebufferObject::ColorAttachment2, g_texIntermediateResult);
			g_framebuffer->AttachTexture(OpenGLFramebufferObject::DepthAttachment, g_texDepth);
			g_framebuffer->CheckFramebufferStatus();
			g_framebuffer->Unbind();
			//init 
			InitBlockExistsHash();
			InitMissedBlockVector();
			{
				g_tfObject.read("d:\\scidata\\std_tf1d.TF1D");
				g_tfObject.FetchData(g_tfData.data(), 256);
				g_texTransferFunction->SetData(OpenGLTexture::RGBA32F, OpenGLTexture::RGBA, OpenGLTexture::Float32, 256,
				                               0, 0, g_tfData.data());
			}
		}
	}
}
#include "gpucacheblockmanager.h"

#include "../opengl/openglutils.h"

namespace ysl
{
	GPUCache::GPUCache(const Size3& cacheSize,void * data, CacheExternalDataFormat extFmt, CacheExternalDataFormat extType,
		CacheInternalDataFormat intFmt):
	OpenGLTexture(Texture3D
		,OpenGLTexture::Linear
		,OpenGLTexture::Linear
		,OpenGLTexture::ClampToEdge
		,OpenGLTexture::ClampToEdge
		,OpenGLTexture::ClampToEdge
		),cacheSize(cacheSize)
	{
		Bind();
		SetData(OpenGLTexture::InternalFormat::R8, OpenGLTexture::ExternalDataFormat::RED, OpenGLTexture::UInt8, cacheSize.x, cacheSize.y, cacheSize.z, data);
		Unbind();
	}

	void HashBasedGPUCacheMissHandler::InitHashBuffer(int blockCount)
	{

		const std::size_t totalBlockCountBytes = blockCount * sizeof(int);
		bufMissedHash = std::make_shared<OpenGLBuffer>(OpenGLBuffer::ShaderStorageBuffer,
			OpenGLBuffer::StreamDraw);
		bufMissedHash->AllocateFor(nullptr, totalBlockCountBytes);
		GL_ERROR_REPORT;
		bufMissedHash->BindBufferBase(0);
	}

	void HashBasedGPUCacheMissHandler::InitMissTableBuffer()
	{
		const auto missedBlockCapacity = 5000 * sizeof(unsigned int);
		bufMissedTable = std::make_shared<OpenGLBuffer>(OpenGLBuffer::ShaderStorageBuffer,
			OpenGLBuffer::StreamDraw);
		bufMissedTable->AllocateFor(nullptr, missedBlockCapacity);
		GL_ERROR_REPORT;
		bufMissedTable->BindBufferBase(1)
	}

	void HashBasedGPUCacheMissHandler::InitGPUAtomicCounter()
	{
		atomicCounter = std::make_shared<OpenGLBuffer>(OpenGLBuffer::AtomicCounterBuffer,
			OpenGLBuffer::DynamicCopy);
		atomicCounter->AllocateFor(nullptr, sizeof(GLuint));
		atomicCounter->BindBufferBase(3);
		atomicCounter->Unbind();
		GL_ERROR_REPORT;
	}

	void HashBasedGPUCacheMissHandler::ResetBlockExistsHash()
	{
		GL_ERROR_REPORT;
		const auto ptr = bufMissedHash->Map(OpenGLBuffer::WriteOnly);
		memset(ptr, 0, bufMissedHash->Size());
		bufMissedHash->Unmap();
		GL_ERROR_REPORT;
	}

	void HashBasedGPUCacheMissHandler::ResetMissedBlockVector()
	{
		const auto ptr = bufMissedTable->Map(OpenGLBuffer::WriteOnly);
		memset(ptr, 0, bufMissedTable->Size());
		bufMissedTable->Unmap();
		atomicCounter->BindBufferBase(3);
		unsigned int zero = 0;
		atomicCounter->AllocateFor(&zero, sizeof(unsigned int));
		GL_ERROR_REPORT
	}

	HashBasedGPUCacheMissHandler::HashBasedGPUCacheMissHandler(int blockCount)
	{
		InitHashBuffer(blockCount);
		InitMissTableBuffer();
		InitGPUAtomicCounter();
	}

	std::vector<VirtualMemoryBlockIndex> HashBasedGPUCacheMissHandler::CaptureCacheMiss()
	{
		const auto counters = static_cast<int*>(atomicCounter->Map(OpenGLBuffer::ReadOnly));
		const int count = counters[0];
		if (count == 0)
			return;

		//const std::size_t cacheBlockThreshold = gpuCacheBlockSize.x * gpuCacheBlockSize.y * gpuCacheBlockSize.z;

		std::vector<VirtualMemoryBlockIndex> hits;

		//const std::size_t blockSize = largeVolumeCache.BlockSize();
		const auto missedBlocks = count;
		//std::cout << count << std::endl;
		GL_ERROR_REPORT;

		hits.clear();
		hits.reserve(missedBlocks);

		const auto ptr = static_cast<int*>(bufMissedTable->Map(OpenGLBuffer::ReadWrite));
		for (auto i = 0; i < missedBlocks; i++)
		{
			const auto blockId = ptr[i];
			hits.emplace_back(blockId, g_xBlockCount, g_yBlockCount, g_zBlockCount);
		}

		bufMissedTable->Unmap();

		return hits;

	}

	void HashBasedGPUCacheMissHandler::Reset()
	{
		ResetBlockExistsHash();
		ResetMissedBlockVector();
	}

	void PingPongTransferManager::InitPingPongSwapPBO()
	{
		assert(vmManager);
		const auto blockDataSize = vmManager->BlockSize();

		pbo[0] = std::make_shared<OpenGLBuffer>(OpenGLBuffer::PixelUnpackBuffer, OpenGLBuffer::StreamDraw);
		pbo[0]->AllocateFor(nullptr, blockDataSize.x * blockDataSize.y * blockDataSize.z * sizeof(char));
		pbo[0]->Unbind();

		pbo[1] = std::make_shared<OpenGLBuffer>(OpenGLBuffer::PixelUnpackBuffer, OpenGLBuffer::StreamDraw);
		pbo[1]->AllocateFor(nullptr, blockDataSize.x * blockDataSize.y * blockDataSize.z * sizeof(char));
		pbo[1]->Unbind();
	}

	PingPongTransferManager::PingPongTransferManager(VirtualMemoryManager * vmm,AbstraGPUCacheMissHandler * gcm):AbstraGPUCacheBlockManager(vmm,gcm)
	{
		InitPingPongSwapPBO();
	}

	void PingPongTransferManager::TransferData(GPUCache * dest, 
		const CPUVolumeDataCache* src)
	{
		const auto hits = gcmHandler->CaptureCacheMiss();

		const auto posInCache = vmManager->UpdatePageTable(hits);

		const auto blockSize = src->BlockSize();
		const auto blockBytes = blockSize * blockSize * blockSize * sizeof(char);
		//std::shared_ptr<OpenGLBuffer> pbo[2] = { blockPingBuf, blockPongBuf };

		const auto missedBlocks = hits.size();

		auto curPBO = 0;
		auto i = 0;
		const auto & idx = hits[i];

		const auto dd = src->ReadBlockDataFromCPUCache(idx);

		pbo[1 - curPBO]->Bind();
		auto pp = pbo[1 - curPBO]->Map(OpenGLBuffer::WriteOnly);
		memcpy(pp, dd, blockBytes);
		pbo[1 - curPBO]->Unmap(); // copy data to pbo
		pbo[1 - curPBO]->Bind();

		dest->Bind();
		for (; i < missedBlocks - 1;)
		{
			pbo[1 - curPBO]->Bind();
			dest->SetSubData(OpenGLTexture::RED,
				OpenGLTexture::UInt8,
				posInCache[i].x, blockSize,
				posInCache[i].y, blockSize,
				posInCache[i].z, blockSize,
				nullptr);
			pbo[1 - curPBO]->Unbind();
			i++;
			const auto& index = hits[i];
			const auto d = src->ReadBlockDataFromCPUCache(index);
			pbo[curPBO]->Bind();
			auto p = pbo[curPBO]->Map(OpenGLBuffer::WriteOnly);
			memcpy(p, d, blockBytes);
			pbo[curPBO]->Unmap(); // copy data to pbo
			curPBO = 1 - curPBO;
			//std::cout << "Pos in cache:" << posInCache[3 * i] << " " << posInCache[3 * i + 1] << " " << posInCache[3 * i + 2] << std::endl;
		}

		pbo[1 - curPBO]->Bind();

		dest->SetSubData(OpenGLTexture::RED,
			OpenGLTexture::UInt8,
			posInCache[i].x, blockSize,
			posInCache[i].y, blockSize,
			posInCache[i].z, blockSize,
			nullptr);

		//std::cout << "Pos in cache:" << posInCache[3 * i] << " " << posInCache[3 * i + 1] << " " << posInCache[3 * i + 2] << std::endl;

		pbo[1 - curPBO]->Unbind();
	}

	PingPongTransferManager::~PingPongTransferManager()
	{

	}
}

#include "gpucacheblockmanager.h"

#include "../opengl/openglutils.h"

#include "abstrgpuobject/abstrgpucache.h"

namespace ysl
{
	GPUVolumeDataCache::GPUVolumeDataCache(const Size3& cacheSize,void * data):
	AbstrGPUCache(OpenGLTexture::Texture3D
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

	void HashBasedGPUCacheFaultHandler::InitHashBuffer(int blockCount)
	{
		const std::size_t totalBlockCountBytes = capacity * sizeof(int);
		bufMissedHash = std::make_shared<OpenGLBuffer>(OpenGLBuffer::ShaderStorageBuffer,
			OpenGLBuffer::StreamDraw);
		bufMissedHash->AllocateFor(nullptr, totalBlockCountBytes);
		GL_ERROR_REPORT;
		bufMissedHash->BindBufferBase(0);
	}

	void HashBasedGPUCacheFaultHandler::InitMissTableBuffer()
	{
		const auto missedBlockCapacity = capacity * sizeof(unsigned int);
		bufMissedTable = std::make_shared<OpenGLBuffer>(OpenGLBuffer::ShaderStorageBuffer,
			OpenGLBuffer::StreamDraw);
		bufMissedTable->AllocateFor(nullptr, missedBlockCapacity);
		GL_ERROR_REPORT;
		bufMissedTable->BindBufferBase(1);
	}

	void HashBasedGPUCacheFaultHandler::InitGPUAtomicCounter()
	{
		atomicCounter = std::make_shared<OpenGLBuffer>(OpenGLBuffer::AtomicCounterBuffer,
			OpenGLBuffer::DynamicCopy);
		atomicCounter->AllocateFor(nullptr, sizeof(GLuint));
		atomicCounter->BindBufferBase(3);
		atomicCounter->Unbind();
		GL_ERROR_REPORT;
	}

	void HashBasedGPUCacheFaultHandler::ResetBlockExistsHash()
	{
		GL_ERROR_REPORT;
		const auto ptr = bufMissedHash->Map(OpenGLBuffer::WriteOnly);
		memset(ptr, 0, bufMissedHash->Size());
		bufMissedHash->Unmap();
		GL_ERROR_REPORT;
	}

	void HashBasedGPUCacheFaultHandler::ResetMissedBlockVector()
	{
		const auto ptr = bufMissedTable->Map(OpenGLBuffer::WriteOnly);
		memset(ptr, 0, bufMissedTable->Size());
		bufMissedTable->Unmap();

		atomicCounter->BindBufferBase(3);
		unsigned int zero = 0;
		atomicCounter->AllocateFor(&zero, sizeof(unsigned int));
		GL_ERROR_REPORT
	}



	HashBasedGPUCacheFaultHandler::HashBasedGPUCacheFaultHandler(int capacity, const Size3& dim):capacity(capacity),dim(dim)
	{
		InitHashBuffer(capacity);
		InitMissTableBuffer();
		InitGPUAtomicCounter();
	}

	std::vector<VirtualMemoryBlockIndex> HashBasedGPUCacheFaultHandler::CaptureCacheFault()
	{
		//atomicCounter->Bind();
		const auto counters = static_cast<int*>(atomicCounter->Map(OpenGLBuffer::ReadOnly));
		const int count = counters[0];

		//atomicCounter->Unmap();
		//atomicCounter->Unbind();

		//std::cout << count << std::endl;
		std::vector<VirtualMemoryBlockIndex> hits;
		if (count == 0)
			return hits;
		const auto missedBlocks = count;
		GL_ERROR_REPORT;
		//hits.clear();
		hits.reserve(missedBlocks);

		const auto ptr = static_cast<int*>(bufMissedTable->Map(OpenGLBuffer::ReadWrite));

		for (auto i = 0; i < missedBlocks; i++)
		{
			const auto blockId = ptr[i];
			hits.emplace_back(blockId,dim.x, dim.y, dim.z);
		}

		bufMissedTable->Unmap();
		GL_ERROR_REPORT;
		return hits;

	}

	void HashBasedGPUCacheFaultHandler::Reset()
	{
		ResetBlockExistsHash();
		ResetMissedBlockVector();

		//std::cout << "Reset:" << std::endl;
	}

	HashBasedGPUCacheFaultHandler::~HashBasedGPUCacheFaultHandler()
	{

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

	PingPongTransferManager::PingPongTransferManager(VirtualMemoryManager * vmm,AbstrGPUCacheFaultHandler * gcm):AbstrGPUCacheBlockManager(vmm,gcm)
	{
		InitPingPongSwapPBO();
	}

	bool PingPongTransferManager::TransferData(GPUVolumeDataCache* dest,
	                                           CPUVolumeDataCache* src)
	{
		assert(gcmHandler);
		assert(vmManager);
		const std::size_t blockSize = src->BlockSize();
		const auto hits = gcmHandler->CaptureCacheFault();
		gcmHandler->Reset();
		const auto missedBlocks = hits.size();
		if (missedBlocks <= 0)
			return false;
		const auto posInCache = vmManager->UpdatePageTable(hits);	 // policy


		const auto blockBytes = blockSize * blockSize * blockSize * sizeof(char);
		// Ping-Pong PBO Transfer
		auto curPBO = 0;
		auto i = 0;
		const auto& idx = hits[i];
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
		}

		pbo[1 - curPBO]->Bind();
		dest->SetSubData(OpenGLTexture::RED,
			OpenGLTexture::UInt8,
			posInCache[i].x, blockSize,
			posInCache[i].y, blockSize,
			posInCache[i].z, blockSize,
			nullptr);
		pbo[1 - curPBO]->Unbind();


		return true;

	}

	PingPongTransferManager::~PingPongTransferManager()
	{

	}
}

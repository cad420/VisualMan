#include "gpucacheblockmanager.h"
#include "../opengl/openglutils.h"
#include "abstrgpuobject/abstrgpucache.h"
#include "virtualvolumehierachy.h"
#include "gpupagetable.h"
#include "gpuvolumedatacache.h"
#include <cassert>

namespace ysl
{
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

	PingPongTransferManager::PingPongTransferManager(std::shared_ptr<PageTableManager> vmm, std::shared_ptr<AbstrGPUCacheFaultHandler> gcm):AbstrGPUCacheBlockManager(vmm,gcm)
	{
		InitPingPongSwapPBO();
	}

	bool PingPongTransferManager::TransferData(GPUVolumeDataCache* dest,
	                                           CPUVolumeDataCache* src)
	{
		assert(gcmHandler);
		assert(vmManager);

		const auto blockSize = src->BlockSize();
		const auto hits = gcmHandler->CaptureCacheFault();
		gcmHandler->Reset();

		const auto missedBlocks = hits.size();
		if (missedBlocks <= 0)
			return false;
		const auto posInCache = vmManager->UpdatePageTable(hits);	 // policy


		const auto blockBytes = blockSize.x * blockSize.y * blockSize.z * sizeof(char);
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
				posInCache[i].x, blockSize.x,
				posInCache[i].y, blockSize.y,
				posInCache[i].z, blockSize.z,
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
			posInCache[i].x, blockSize.x,
			posInCache[i].y, blockSize.y,
			posInCache[i].z, blockSize.z,
			nullptr);
		pbo[1 - curPBO]->Unbind();
		return true;
	}

	PingPongTransferManager::~PingPongTransferManager()
	{

	}
}

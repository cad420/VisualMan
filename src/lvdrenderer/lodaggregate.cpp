
#include <largevolumecache.h>

#include "lodaggregate.h"
#include "gpucachefaulthandler.h"
#include "gpuvolumedatacache.h"
#include "gpucacheblockmanager.h"
#include "openglutils.h"

namespace ysl
{
	void LODAggregate::InitGPUPageTableBuffer()
	{
		pageTableManager = std::make_shared<PageTableManager>(texCache, largeVolumeCache);
	}

	void LODAggregate::InitGPUBlockCacheTexture(const Size3 & blockDim)
	{
		texCache = std::make_shared<GPUVolumeDataCache>(blockDim, largeVolumeCache->BlockSize(), nullptr);
	}

	bool LODAggregate::CaptureAndHandleCacheFault()
	{
		const auto flag = pingpongTransferManager->TransferData(texCache.get(), largeVolumeCache.get());
		if (!flag)
			return false;
		pageTableManager->UpdatePageTable();
		return true;
	}

	LODAggregate::LODAggregate(const std::string& fileName,const Size3 & blockDim):largeVolumeCache(std::make_shared<MemoryPageAdapter>(fileName))
	{
		InitGPUBlockCacheTexture(blockDim);
		InitGPUPageTableBuffer();

		cacheFaultHandler = std::make_shared<HashBasedGPUCacheFaultHandler>(5000, largeVolumeCache->BlockDim());
		pingpongTransferManager = std::make_shared<PingPongTransferManager>(pageTableManager, cacheFaultHandler);
		GL_ERROR_REPORT;
	}

	void LODAggregate::Bind(const SHADERBINDINGPOINT& bp)
	{
		pageTableManager->BindTextureToImage(bp.PAGE_TABLE_CACHE_BINDING_POINT);
		cacheFaultHandler->BindHashTableTo(bp.HASH_TABLE_BUFFER_BINDING_POINT);
		cacheFaultHandler->BindAtomicCounterTo(bp.ATOMIC_COUNTER_BINDING_POINT);
		cacheFaultHandler->BindFaultTableTo(bp.FAULT_TABLE_BUFFER_BINDING_POINT);
	}


	std::shared_ptr<GPUVolumeDataCache> LODAggregate::GPUCache() const
	{
		return texCache;
	}

	std::shared_ptr<MemoryPageAdapter> LODAggregate::CPUCache() const
	{
		return largeVolumeCache;
	}

	Size3 LODAggregate::PageTableSize() const
	{
		return pageTableManager->PageTableSize();
	}

	Size3 LODAggregate::OriginalDataSize() const
	{
		return largeVolumeCache->DataSizeWithoutPadding();
	}

	Size3 LODAggregate::BlockSize() const
	{
		return largeVolumeCache->BlockSize();
	}

	Size3 LODAggregate::CPUBlockDim() const
	{
		return largeVolumeCache->BlockDim();
	}

	Size3 LODAggregate::CPUCacheSize() const
	{
		return CPUBlockDim() * BlockSize();
	}

	Size3 LODAggregate::GPUBlockDim() const
	{
		return texCache->BlockDim();
	}

	Size3 LODAggregate::GPUCacheSize() const
	{
		return GPUBlockDim() * BlockSize();
	}

	int LODAggregate::Padding() const
	{
		return largeVolumeCache->Padding();
	}

}

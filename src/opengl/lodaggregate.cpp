#include "lodaggregate.h"
#include "../volume/largevolumecache.h"
#include "gpucachefaulthandler.h"
#include "gpuvolumedatacache.h"
#include "gpucacheblockmanager.h"
#include "openglutils.h"

namespace ysl
{
	void LODAggregate::InitGPUPageTableBuffer()
	{
		
	}

	void LODAggregate::InitGPUBlockCacheTexture(const Size3 & blockDim)
	{
		
	}

	bool LODAggregate::CaptureAndHandleCacheFault()
	{
		const auto flag = pingpongTransferManager->TransferData(texCache.get(), largeVolumeCache.get());
		if (!flag)
			return false;
		pageTableManager->UpdatePageTable();
		return true;
	}

	LODAggregate::LODAggregate(const std::string& fileName,const Size3 & blockDim)
	{
		largeVolumeCache = std::make_shared<CPUVolumeDataCache>(fileName);
		texCache = std::make_shared<GPUVolumeDataCache>(blockDim, largeVolumeCache->BlockSize(), nullptr);
		pageTableManager = std::make_shared<PageTableManager>(texCache, largeVolumeCache);
		
		const auto d = largeVolumeCache->CacheBlockDim();
		const auto hashSize = 1000;
		cacheFaultHandler = std::make_shared<HashBasedGPUCacheFaultHandler>(hashSize, largeVolumeCache->BlockDim());
		pingpongTransferManager = std::make_shared<PingPongTransferManager>(pageTableManager, cacheFaultHandler);
		GL_ERROR_REPORT;
	}

	void LODAggregate::Bind(const SHADERBINDINGPOINT& bp)
	{
		cacheFaultHandler->BindHashTableTo(bp.HASH_TABLE_BUFFER_BINDING_POINT);
		cacheFaultHandler->BindAtomicCounterTo(bp.ATOMIC_COUNTER_BINDING_POINT);
		cacheFaultHandler->BindFaultTableTo(bp.FAULT_TABLE_BUFFER_BINDING_POINT);
		pageTableManager->BindTextureToImage(bp.PAGE_TABLE_CACHE_BINDING_POINT);

	}

	std::shared_ptr<GPUVolumeDataCache> LODAggregate::TextureCache() const
	{
		return texCache;
	}

	Size3 LODAggregate::PageTableSize() const
	{
		return pageTableManager->PageTableSize();
	}

	Size3 LODAggregate::OriginalDataSize() const
	{
		return largeVolumeCache->OriginalDataSize();
	}

	Size3 LODAggregate::BlockSize() const
	{
		return largeVolumeCache->BlockSize();
	}

	int LODAggregate::Padding() const
	{
		return largeVolumeCache->Padding();
	}
}

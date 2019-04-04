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
		pageTableManager = std::make_shared<PageTableManager>(texCache, largeVolumeCache);
	}

	void LODAggregate::InitGPUBlockCacheTexture(const Size3 & blockDim)
	{
		texCache = std::make_shared<GPUVolumeDataCache>(Size3{10,10,10}, largeVolumeCache->BlockSize(), nullptr);
	}

	bool LODAggregate::CaptureAndHandleCacheFault()
	{
		const auto flag = pingpongTransferManager->TransferData(texCache.get(), largeVolumeCache.get());
		if (!flag)
			return false;

		pageTableManager->UpdatePageTable();

		return true;
	}

	LODAggregate::LODAggregate(const std::string& fileName,const Size3 & blockDim):largeVolumeCache(std::make_shared<CPUVolumeDataCache>(fileName))
	{
		InitGPUBlockCacheTexture(blockDim);
		InitGPUPageTableBuffer();

		cacheFaultHandler = std::make_shared<HashBasedGPUCacheFaultHandler>(5000, largeVolumeCache->BlockDim());
		pingpongTransferManager = std::make_shared<PingPongTransferManager>(pageTableManager, cacheFaultHandler);
		GL_ERROR_REPORT;
	}

	void LODAggregate::Bind(const ShaderBindingPoint& bp)
	{
		pageTableManager->BindTextureToImage(bp.PAGE_TABLE_CACHE_BINDING_POINT);

	}
}

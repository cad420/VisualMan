
#include "virtualmemorymanager.h"
namespace ysl
{
	void AbstrMemoryCache::SetNextLevelCache(std::shared_ptr<AbstrMemoryCache> cache)
	{
		nextLevel = cache;
	}

	void AbstrMemoryCache::SetCachePolicy(std::unique_ptr<AbstrCachePolicy> policy)
	{
		if (!policy)
			return;
		if (cachePolicy)
		{
			cachePolicy->SetOwnerCache(nullptr);
		}
		cachePolicy = std::move(policy);
		cachePolicy->SetOwnerCache(this);
		cachePolicy->InitEvent(this);
	}

	std::unique_ptr<AbstrCachePolicy> AbstrMemoryCache::TakeCachePolicy()
	{
		if (cachePolicy)
		{
			cachePolicy->SetOwnerCache(nullptr);
			return std::move(cachePolicy);
		}
		return nullptr;
	}

	const void* AbstrMemoryCache::GetPage(size_t pageID)
	{
		assert(cachePolicy);
		const bool e = cachePolicy->QueryPage(pageID);
		if (!e)
		{
			const auto storageID = cachePolicy->QueryAndUpdate(pageID);
			// Read block from next level to the storage cache
			memcpy(GetPageStorage_Implement(storageID), nextLevel->GetPage(pageID), GetPageSize());
			return GetPageStorage_Implement(storageID);
		}
		else {
			const auto storageID = cachePolicy->QueryAndUpdate(pageID);
			return GetPageStorage_Implement(storageID);
		}
	}
}
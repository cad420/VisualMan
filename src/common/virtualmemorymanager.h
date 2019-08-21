
#ifndef _VIRTUALMEMORYMANAGER_H_
#define _VIRTUALMEMORYMANAGER_H_

//#include "largevolumecache.h"
#include "common.h"
#include "geometry.h"
#include <memory>
#include <cstring>

namespace ysl
{
	struct PageDirectoryEntryAbstractIndex
	{
		using internal_type = int;
		const internal_type x, y, z;
		PageDirectoryEntryAbstractIndex(internal_type x_ = -1,
			internal_type y_ = -1,
			internal_type z_ = -1)
			:x(x_), y(y_), z(z_) {}
	};

	struct PageTableEntryAbstractIndex
	{
		using internal_type = int;
		internal_type x, y, z;
		internal_type lod = 0;
		PageTableEntryAbstractIndex(internal_type x_ = -1,
			internal_type y_ = -1,
			internal_type z_ = -1) :
			x(x_), y(y_), z(z_) {}

	};

	struct PhysicalMemoryBlockIndex			// DataBlock start in 3d texture
	{
		using internal_type = int;
		const internal_type x, y, z;
	private:
		uint8_t unit = 0;
	public:
		PhysicalMemoryBlockIndex(internal_type x_ = -1,
			internal_type y_ = -1,
			internal_type z_ = -1) :
			x(x_), y(y_), z(z_), unit(0) {}
		PhysicalMemoryBlockIndex(internal_type x_,
			internal_type y_,
			internal_type z_,
			uint8_t unit) :
			x(x_), y(y_), z(z_), unit(unit) {}
		int GetPhysicalStorageUnit()const { return unit; }
		void SetPhysicalStorageUnit(uint8_t u) { unit = u; }
		Vec3i ToVec3i()const { return Vec3i{ x,y,z }; }
	};

	struct VirtualMemoryBlockIndex
	{
		VirtualMemoryBlockIndex() = default;
		VirtualMemoryBlockIndex(std::size_t linearId, int xb, int yb, int zb)
		{
			z = linearId / (xb*yb);
			y = (linearId - z * xb*yb) / xb;
			x = linearId - z * xb*yb - y * xb;
		}
		VirtualMemoryBlockIndex(int x, int y, int z) :x(x), y(y), z(z) {}
		Vec3i ToVec3i()const { return Vec3i{ x,y,z }; }

		using index_type = int;
		index_type x = -1, y = -1, z = -1;
	};

	class AbstrCachePolicy;

	class COMMON_EXPORT_IMPORT AbstrMemoryCache
	{
		std::shared_ptr<AbstrMemoryCache> nextLevel;
		std::unique_ptr<AbstrCachePolicy> cachePolicy;
	public:
		void SetNextLevelCache(std::shared_ptr<AbstrMemoryCache> cache);
		void SetCachePolicy(std::unique_ptr<AbstrCachePolicy> policy);
		std::unique_ptr<AbstrCachePolicy> TakeCachePolicy();
		std::shared_ptr<AbstrMemoryCache> GetNextLevelCache() { return nextLevel; }
		std::shared_ptr<const AbstrMemoryCache> GetNextLevelCache()const { return nextLevel; }

		/**
		 * \brief Get the page give by \a pageID. If the page does not exist in the cache, it will be swapped in.
		 * \note The page data pointed by the  pointer returned by the function is only valid at current call.
		 * It could be invalid when next call because its data has been swapped out.
		 */
		virtual const void* GetPage(size_t pageID);

		/**
		 * \brief Returns the page size by bytes
		 */
		virtual size_t GetPageSize() = 0;
		/**
		 * \brief 
		 * \return 
		 */
		virtual size_t GetPhysicalPageCount() = 0;
		/**
		 * \brief 
		 * \return 
		 */
		virtual size_t GetVirtualPageCount() = 0;

		virtual ~AbstrMemoryCache() = default;
	private:
		/**
		 * \brief 
		 * \param pageID 
		 * \return 
		 */
		virtual void * GetPageStorage_Implement(size_t pageID) = 0;

	};


	class COMMON_EXPORT_IMPORT AbstrCachePolicy:public AbstrMemoryCache
	{
	public:
		/**
		 * \brief Queries the page given by \a pageID if it exists in storage cache. Returns \a true if it exists or \a false if not
		 */
		virtual bool QueryPage(size_t pageID) = 0;
		/**
		 * \brief Updates the fault page given by \a pageID. Returns the actually storage ID of the page. If the page exists, the function does nothing.
		 */
		virtual void UpdatePage(size_t pageID) = 0;
		/**
		 * \brief Queries and updates at the same time. It will always return a valid storage id.
		 */
		virtual size_t QueryAndUpdate(size_t pageID) = 0;

		AbstrMemoryCache* GetOwnerCache() { return ownerCache; }

		const AbstrMemoryCache * GetOwnerCache()const { return ownerCache; }

		const void * GetPage(size_t pageID) override { return nullptr; }

		size_t GetPageSize() override { return 0; }

		size_t GetPhysicalPageCount() override { return 0; }

		size_t GetVirtualPageCount() override { return 0; }
	protected:
		void * GetPageStorage_Implement(size_t pageID) override { return nullptr; }

		virtual void InitEvent(AbstrMemoryCache * cache) = 0;
	private:
		friend AbstrMemoryCache;

		AbstrMemoryCache * ownerCache = nullptr;

		void SetOwnerCache(AbstrMemoryCache * cache) { ownerCache = cache; }
	};
}



#endif

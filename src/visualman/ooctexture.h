
#ifndef _CACHETEXTURE_H_
#define _CACHETEXTURE_H_
#include "texture.h"
#include <vector>
#include "largevolumecache.h"
#include "oocprimitive.h"

namespace ysl
{
	namespace vm
	{
		struct SubDataDescriptor
		{
		private:
			void * data = nullptr;
			PhysicalMemoryBlockIndex key;
			size_t blockId = 0;
		public:
			SubDataDescriptor() = default;
			SubDataDescriptor(void * d, const PhysicalMemoryBlockIndex & key,size_t id) :data(d), key(key),blockId(id) {}
			void * Value()const { return data; }
			PhysicalMemoryBlockIndex & Key() { return key; }
			const PhysicalMemoryBlockIndex & Key()const { return key; }
			size_t BlockId()const { return blockId; }
		};

		enum EntryFlag { Empty = 0, Unmapped = 2, Mapped = 1 };

		class MappingTableManager
		{

		public:
			struct PageDirEntry
			{
				int x, y, z, w;
			};
			struct PageTableEntry
			{
				int x, y, z, w;
			};
		private:
			//Linear3DArray<PageDirEntry> PageDir;
			Linear3DArray<PageTableEntry> pageTable;
			std::list<std::pair<PageTableEntryAbstractIndex, PhysicalMemoryBlockIndex>> g_lruList;
			//const Size3 physicalMemoryBlock;
			//const Size3 virtualMemoryBlock;
			//const Size3 blockSize;
			//CPUVolumeDataCache * const cacheData;
			//const std::shared_ptr<CPUVolumeDataCache> cacheData;
			void InitCPUPageTable(const Size3& blockDim);
			void InitLRUList(const Size3& physicalMemoryBlock, const Size3& page3DSize);
			void InitLRUList(const Size3& physicalMemoryBlock);
		public:
			using size_type = std::size_t;
			//PageTableManager(const Size3 & physicalMemoryBlock, std::shared_ptr<CPUVolumeDataCache> virtualData): physicalMemoryBlock(physicalMemoryBlock),
			//virtualMemoryBlock(virtualData->BlockDim()),
			//blockSize(virtualData->BlockSize()),
			//cacheData(virtualData)
			//{

			//	InitCPUPageTable(virtualMemoryBlock);
			//	InitLRUList(physicalMemoryBlock, blockSize);
			//	InitGPUPageTable();
			//}

			//MappingTableManager(std::shared_ptr<CPUVolumeDataCache> virtualData)
			//	//cacheData(std::move(virtualData)),
			//	//gpuCacheData(std::move(physicalData))
			//{

			//	//InitCPUPageTable(TODO:);    // block size
			//	//InitLRUList(TODO:,TODO:);	// blocks zie ,page table size}
			//}

			/**
			 * \brief 
			 * \param virtualSpaceSize virtual space size
			 */
			MappingTableManager(const Size3 & virtualSpaceSize)
			{
				InitCPUPageTable(virtualSpaceSize);
				InitLRUList(virtualSpaceSize);
			}


			const void * GetData()const { return pageTable.Data(); }

			/**
			 * \brief Translates the virtual space address to the physical address and update the mapping table by LRU policy
			 */
			std::vector<PhysicalMemoryBlockIndex> UpdatePageTable(const std::vector<VirtualMemoryBlockIndex>& missedBlockIndices);
		};

		class VISUALMAN_EXPORT_IMPORT OutOfCoreVolumeTexture :public IOutOfCoreAdapter			// Dest
		{
		public:
			explicit OutOfCoreVolumeTexture(const std::string & fileName);

			void OnDrawCallStart(OutOfCorePrimitive* p) override;
			void OnDrawCallFinished(OutOfCorePrimitive* p) override;

			Ref<Texture> GetVolumeTexture() { return volumeData; }
			Ref<const Texture> GetVolumeTexture()const { return volumeData; }

			Ref<BufferObject> GetAtomicCounterBuffer() { return atomicCounterBuffer; }
			Ref<const BufferObject> GetAtomicCounterBuffer()const { return atomicCounterBuffer; }

			Ref<BufferObject> GetBlockIDBuffer() { return blockIdBuffer; }
			Ref<const BufferObject> GetBlockIDBuffer()const { return blockIdBuffer; }

			Ref<BufferObject> GetHashBuffer() { return duplicateRemoveHash; }
			Ref<const BufferObject> GetHashBuffer()const { return duplicateRemoveHash; }

			Ref<Texture> GetMappingTableTexture() { return mappingTable; }
			Ref<const Texture> GetMappingTableTexture()const { return mappingTable; }

			Vec3i DataResolution()const { return Vec3i(cpuVolumeData->OriginalDataSize()); }
			Vec3i DataResolutionWithPadding()const { return Vec3i(cpuVolumeData->BlockDim()*cpuVolumeData->BlockSize()); }
			Vec3i Padding()const {return Vec3i( cpuVolumeData->Padding(),cpuVolumeData->Padding(),cpuVolumeData->Padding()); }
			Vec3i VirtualBlockDim()const { return Vec3i(cpuVolumeData->BlockDim()); }
			Vec3i BlockSize()const { return Vec3i(cpuVolumeData->BlockSize());}

			void BindToOutOfCorePrimitive(Ref<OutOfCorePrimitive> oocPrimitive);
			~OutOfCoreVolumeTexture();
		private:
			static constexpr int pboCount = 3;

			void SetSubTextureDataUsePBO(const std::vector<SubDataDescriptor> & data);

			Size3 EvalTextureSize(const Size3 & hint)const;
			size_t EvalIDBufferSize(const Size3 & hint)const;

			//std::array<unsigned int, 2> pbo;
			void CreatePBOs(int bytes);
			void DestroyPBOs();
			unsigned int pbo=0;
			std::array<void*, pboCount> pboPtrs;
			std::array<void*, pboCount> offset;
			std::array<GLsync, pboCount> fences;


			size_t bytes = 0;
			Ref<Texture> volumeData;
			Ref<CPUVolumeDataCache> cpuVolumeData;
			Ref<BufferObject> atomicCounterBuffer;
			Ref<BufferObject> duplicateRemoveHash;

			Ref<BufferObject> blockIdBuffer;
			std::vector<int> blockIdLocalBuffer;

			Ref<Texture> mappingTable;
			Ref<MappingTableManager> mappingTableManager;

			// CPU Volume Cache
		};
	}
}

#endif
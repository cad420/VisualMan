
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
		struct BlockDescriptor
		{
		private:
			PhysicalMemoryBlockIndex value;
			VirtualMemoryBlockIndex key;
		public:
			BlockDescriptor() = default;
			BlockDescriptor(const PhysicalMemoryBlockIndex & value,VirtualMemoryBlockIndex key) :value(value),key(key) {}
			const PhysicalMemoryBlockIndex & Value()const { return value; }
			const VirtualMemoryBlockIndex & Key()const { return key; }
		};

		enum EntryMapFlag { EM_UNKNOWN = 0, EM_UNMAPPED = 2, EM_MAPPED = 1 };


		struct IVideoMemoryParamsEvaluator 
		{
			virtual Size3 EvalPhysicalTextureSize()const = 0;
			virtual int EvalPhysicalTextureCount()const = 0;
			virtual Size3 EvalPhysicalBlockDim()const = 0;
			virtual int EvalHashBufferSize()const = 0;
			virtual int EvalIDBufferCount()const = 0;
			virtual ~IVideoMemoryParamsEvaluator()=default;
		};

		struct DefaultMemoryParamsEvaluator:IVideoMemoryParamsEvaluator
		{
		private:
			const Size3 virtualDim;
			const Size3 blockSize;
		public:
			DefaultMemoryParamsEvaluator(const Size3 & virtualDim, const Size3 & blockSize) :virtualDim(virtualDim), blockSize(blockSize) {}
			Size3 EvalPhysicalTextureSize() const override;
			Size3 EvalPhysicalBlockDim() const override;
			int EvalPhysicalTextureCount() const override;
			int EvalHashBufferSize() const override;
			int EvalIDBufferCount() const override;
			~DefaultMemoryParamsEvaluator() = default;
		};

		class MappingTableManager
		{
		public:
			struct PageDirEntry
			{
				int x, y, z, w;

			};
			struct PageTableEntry
			{
				int x, y, z;
			private:
				int w = 0;
			public:
				void SetMapFlag(EntryMapFlag f) { w = (w & (0xFFF0)) | ((0xF)&f); }// [0,4) bits
				void SetTextureUnit(int unit) { w = (w&0xFF0F) | ((0xF&unit)<<4); } // [4,8) bits
				EntryMapFlag GetMapFlag()const { return EntryMapFlag((0xF)&w); }
				int GetTextureUnit()const { return (w >> 4)&0xF; }
			};
		private:
			using LRUList = std::list<std::pair<PageTableEntryAbstractIndex, PhysicalMemoryBlockIndex>>;
			using LRUMap = std::unordered_map<size_t, LRUList::iterator>;
			Linear3DArray<PageTableEntry> pageTable;

			LRUMap lruMap;
			LRUList g_lruList;

			void InitCPUPageTable(const Size3& blockDim,void * external);
			void InitLRUList(const Size3& physicalMemoryBlock,int unitCount);
		public:
			using size_type = std::size_t;
			/**
			 * \brief 
			 * \param virtualSpaceSize virtual space size
			 */
			MappingTableManager(const Size3& virtualSpaceSize, const Size3& physicalSpaceSize);

			MappingTableManager(const Size3& virtualSpaceSize, const Size3& physicalSpaceSize, int physicalSpaceCount);

			MappingTableManager(const Size3 & virtualSpaceSize, const Size3 & phsicalSpaceSize, int physicalSpaceCount, void * external);


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

			Ref<Texture> GetVolumeTexture() { return volumeDataTexture[0]; }
			Ref<const Texture> GetVolumeTexture()const { return volumeDataTexture[0]; }

			Ref<Texture> GetVolumeTexture(int unit) { return volumeDataTexture[unit]; }
			Ref<const Texture> GetVolumeTexture(int unit)const { return volumeDataTexture[unit]; }

			Ref<BufferObject> GetAtomicCounterBuffer() { return atomicCounterBuffer; }
			Ref<const BufferObject> GetAtomicCounterBuffer()const { return atomicCounterBuffer; }

			Ref<BufferObject> GetBlockIDBuffer() { return blockIdBuffer; }
			Ref<const BufferObject> GetBlockIDBuffer()const { return blockIdBuffer; }

			Ref<BufferObject> GetHashBuffer() { return hashBuffer; }
			Ref<const BufferObject> GetHashBuffer()const { return hashBuffer; }

			Ref<BufferObject> GetPageTableBuffer(){ return pageTableBuffer; }
			Ref<const BufferObject> GetPageTableBuffer()const { return pageTableBuffer; }

			//Ref<Texture> GetPageTableTexture() { return mappingTable; }
			//Ref<const Texture> GetPageTableTexture()const { return mappingTable; }

			Ref<BufferObject> GetSamplerUBO() { return volumeTexSamplerUBO; }
			Ref<const BufferObject> GetSamplerUBO()const { return volumeTexSamplerUBO; }


			Vec3i DataResolution()const { return Vec3i(cpuVolumeData->OriginalDataSize()); }
			Vec3i DataResolutionWithPadding()const { return Vec3i(cpuVolumeData->BlockDim()*cpuVolumeData->BlockSize()); }
			Vec3i Padding()const {return Vec3i( cpuVolumeData->Padding(),cpuVolumeData->Padding(),cpuVolumeData->Padding()); }
			Vec3i VirtualBlockDim()const { return Vec3i(cpuVolumeData->BlockDim()); }
			Vec3i BlockSize()const { return Vec3i(cpuVolumeData->BlockSize());}

			void PrintVideoMemoryUsageInfo();

			void BindToOutOfCorePrimitive(Ref<OutOfCorePrimitive> oocPrimitive);
			~OutOfCoreVolumeTexture();
		private:
			static constexpr int pboCount = 3;
			static constexpr GLbitfield mapping_flags = GL_MAP_WRITE_BIT | GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
			static constexpr GLbitfield	storage_flags = GL_DYNAMIC_STORAGE_BIT | mapping_flags;
			void SetSubTextureDataUsePBO(const std::vector<BlockDescriptor> & data);

			void InitVolumeTextures();
			void InitMappingTable();

			double time = 0;
			size_t totalBlocks = 0;
			size_t bytes = 0;

			std::vector<Ref<Texture>> volumeDataTexture;
			Ref<MemoryPageAdapter> cpuVolumeData;
			Ref<BufferObject> atomicCounterBuffer;
			Ref<BufferObject> hashBuffer;
			Ref<BufferObject> blockIdBuffer;
			Ref<BufferObject> volumeTexSamplerUBO;
			Ref<BufferObject> pageTableBuffer;

			std::vector<int> blockIdLocalBuffer;
			Ref<Texture> mappingTable;
			Ref<MappingTableManager> mappingTableManager;
			Ref<IVideoMemoryParamsEvaluator> memoryEvalator;
			// CPU Volume Cache
		};
	}
}

#endif
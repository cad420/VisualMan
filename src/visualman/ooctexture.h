
#ifndef _CACHETEXTURE_H_
#define _CACHETEXTURE_H_
#include "texture.h"
#include <vector>
#include "largevolumecache.h"
#include "oocprimitive.h"
//#include "rapidjson/reader.h"

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
		//	virtual int EvalHashBufferSize()const = 0;
		//	virtual int EvalIDBufferCount()const = 0;
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
			//int EvalHashBufferSize() const override;
		//	int EvalIDBufferCount() const override;
			~DefaultMemoryParamsEvaluator() = default;
		};

		struct LODPageTableInfo
		{
			Vec3i virtualSpaceSize;
			void * external = nullptr;
			size_t offset = 0;
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

			std::vector<Linear3DArray<PageTableEntry>> lodPageTables;

			LRUMap lruMap;
			LRUList lruList;

			void InitCPUPageTable(const Size3& blockDim,void * external);
			void InitLRUList(const Size3& physicalMemoryBlock,int unitCount);

			//void InitCPUPageTable();

		public:
			using size_type = std::size_t;
			/**
			 * \brief 
			 * \param virtualSpaceSize virtual space size
			 */
			MappingTableManager(const Size3& virtualSpaceSize, const Size3& physicalSpaceSize);

			MappingTableManager(const Size3& virtualSpaceSize, const Size3& physicalSpaceSize, int physicalSpaceCount);

			MappingTableManager(const Size3 & virtualSpaceSize, const Size3 & phsicalSpaceSize, int physicalSpaceCount, void * external);

			MappingTableManager(const std::vector<LODPageTableInfo> &infos, const Size3& physicalSpaceSize,int physicalSpaceCount);


			const void * GetData()const { return pageTable.Data(); }

			size_t GetBytes(int lod) { return lodPageTables[lod].Size().Prod() * sizeof(PageTableEntry); }
			/**
			 * \brief Translates the virtual space address to the physical address and update the mapping table by LRU policy
			 */
			std::vector<PhysicalMemoryBlockIndex> UpdatePageTable(const std::vector<VirtualMemoryBlockIndex> & missedBlockIndices);
			std::vector<PhysicalMemoryBlockIndex> UpdatePageTable(int lod,const std::vector<VirtualMemoryBlockIndex> & missedBlockIndices);

			//std::vector<PhysicalMemoryBlockIndex> UpdatePageTable(const std::vector<size_t> & missedBlockIndices);
		};

		struct LVDFileInfo
		{
			std::vector<std::string> fileNames;
			float samplingRate = 0.001;
			Vec3f spacing = Vec3f{ 1.f,1.f,1.f };
		};

		struct _std430_layout_LODInfo
		{
			Vec3i pageTableSize;
			uint32_t pageTableOffset;
			uint32_t hashBufferOffset;
			uint32_t idBufferOffset;
		};

		VISUALMAN_EXPORT_IMPORT LVDFileInfo GetLVDFileInfoFromJson(const std::string & fileName);

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

			Ref<BufferObject> GetLODInfoBuffer() { return lodInfoBuffer; }
			Ref<const BufferObject> GetLODInfoBuffer()const { return lodInfoBuffer; }

			int GetLODCount()const { return lodCount; };


			[[deprecated]]
			Vec3i DataResolution()const { return Vec3i(cpuVolumeData[0]->OriginalDataSize()); }

			[[deprecated]]
			Vec3i DataResolutionWithPadding()const { return Vec3i(cpuVolumeData[0]->BlockDim()*cpuVolumeData[0]->BlockSize()); }

			[[deprecated]]
			Vec3i Padding()const {return Vec3i( cpuVolumeData[0]->Padding(),cpuVolumeData[0]->Padding(),cpuVolumeData[0]->Padding()); }

			[[deprecated]]
			Vec3i VirtualBlockDim()const { return Vec3i(cpuVolumeData[0]->BlockDim()); }

			[[deprecated]]
			Vec3i BlockSize()const { return Vec3i(cpuVolumeData[0]->BlockSize());}

			void PrintVideoMemoryUsageInfo();

			void BindToOutOfCorePrimitive(Ref<OutOfCorePrimitive> oocPrimitive);
			~OutOfCoreVolumeTexture();
		private:
			static constexpr int pboCount = 3;
			static constexpr GLbitfield mapping_flags = GL_MAP_WRITE_BIT | GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
			static constexpr GLbitfield	storage_flags = GL_DYNAMIC_STORAGE_BIT | mapping_flags;
			void SetSubTextureDataUsePBO(const std::vector<BlockDescriptor> & data);

			void InitVolumeTextures();


			double time = 0;
			size_t totalBlocks = 0;
			size_t bytes = 0;




			std::vector<Ref<Texture>> volumeDataTexture;

			std::vector<Ref<MemoryPageAdapter>> cpuVolumeData;


			/**
			 * \brief Stores the atomic counters for every lod data
			 * 
			 * Using the first 4 bytes to store the atomic counter storage for the LOD0, 
			 * and the second 4 bytes for the second LOD, etc.
			 *  
			 */
			Ref<BufferObject> atomicCounterBuffer;
			/**
			 * \brief Stores the hash table for every lod data.
			 * 
			 * Using the first section of the hash buffer to store the hash table for the LOD0, 
			 * and the second section of the hash buffer for the second LOD, etc.
			 */
			Ref<BufferObject> hashBuffer;
			/**
			 * \brief Stores the missed block id for every lod data
			 * 
			 * Using the first section of the id buffer to store the missed block id for the LOD0, 
			 * and the second section of the hash buffer for the second LOD, etc.
			 */
			Ref<BufferObject> blockIdBuffer;
			/**
			 * \brief Stores the page table for every lod data
			 *
			 * Using the first section of the page table buffer to store the page table for the LOD0, 
			 * and the second section of the page table buffer for the second LOD, etc.
			 */
			Ref<BufferObject> pageTableBuffer;
			/**
			 * \brief Stores all the lod information.
			 * 
			 * 
			 * 
			 * \note The memory layout of the buffer is GLSL-dependent. See the definition of \a LODInfo in the fragment shader blockraycasting_f.glsl
			 */
			Ref<BufferObject> lodInfoBuffer;

			std::vector<int> blockIdLocalBuffer;

			/**
			 * \brief Manages and updates the LOD mapping tables.
			 */
			Ref<MappingTableManager> mappingTableManager;

			//std::vector<LODPageTableInfo> pageTableInfos;

			//std::vector<Ref<IVideoMemoryParamsEvaluator>> memoryEvaluators;
			Ref<IVideoMemoryParamsEvaluator> memoryEvaluators;
			// CPU Volume Cache

			/**
			 * \brief See the definition of the LODInfo in blockraycasting_f.glsl
			 *
			 */


			std::vector<_std430_layout_LODInfo> lodInfo;
			int lodCount = 1;
		};
	}
}

#endif
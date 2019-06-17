
#include "ooctexture.h"
#include <GL/gl3w.h>
#include <cstring>
#include "timer.h"

//#include "../lvdrenderer/virtualvolumehierachy.h"

namespace ysl
{
	namespace vm
	{
		void OutOfCoreVolumeTexture::BindToOutOfCorePrimitive(Ref<OutOfCorePrimitive> oocPrimitive)
		{
			if (oocPrimitive)
			{
				oocPrimitive->RemoveOutOfCoreResources(shared_from_this());
				oocPrimitive->SetOutOfCoreResources(shared_from_this());
			}
		}

		OutOfCoreVolumeTexture::~OutOfCoreVolumeTexture()
		{
			DestroyPBOs();
		}

		void OutOfCoreVolumeTexture::SetSubTextureDataUsePBO(const std::vector<SubDataDescriptor>& data)
		{
			// Ping-Pong PBO Transfer
			assert(volumeData->Handle());


			const auto blockSize = cpuVolumeData->BlockSize();
			const auto textureId = volumeData->Handle();

			const auto blockBytes = this->bytes;

			auto curPBO = 0;
			auto i = 0;
			//const auto dd = data[0].Value();
			const auto dd = cpuVolumeData->ReadBlockDataFromCPUCache(data[i].BlockId());
			GL(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo));

			///**********************************************/

			auto pp = pboPtrs[curPBO];
			memcpy(pp, dd, blockBytes);


			//for (; i < data.size() - 1;)
			//{
			//	const auto & blockIndex = data[i].Key();
			//	const auto posInCache = Vec3i(blockIndex.x*blockSize.x, blockIndex.y*blockSize.y, blockIndex.z*blockSize.z);
			//	GL(glTextureSubImage3D(textureId, 0, posInCache.x, posInCache.y, posInCache.z, blockSize.x, blockSize.y, blockSize.z, IF_RED, GL_UNSIGNED_BYTE, offset[curPBO]));
			//	i++;
			//	const auto d = cpuVolumeData->ReadBlockDataFromCPUCache(data[i].BlockId());
			//	const auto nextPBO = (i) % pboCount;
			//	auto p = pboPtrs[nextPBO];
			//	memcpy(p, d, blockBytes);
			//	curPBO = nextPBO;
			//	{
			//		GLsync s = 0;
			//		GL(s = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0));
			//		GL(glClientWaitSync(s, 0, 100000000));
			//		GL(glDeleteSync(s));
			//	}
			//}

			////const auto & posInCache = data[i].Key();
			//const auto & blockIndex = data[i].Key();
			//const auto posInCache = Vec3i(blockIndex.x*blockSize.x, blockIndex.y*blockSize.y, blockIndex.z*blockSize.z);

			////GL(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo[1 - curPBO]));
			//GL(glTextureSubImage3D(textureId, 0, posInCache.x, posInCache.y, posInCache.z, blockSize.x, blockSize.y, blockSize.z, IF_RED, GL_UNSIGNED_BYTE, offset[curPBO]));
			//{
			//	GLsync s = 0;
			//	GL(s = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0));
			//	GL(glClientWaitSync(s, 0, 100000000));
			//	GL(glDeleteSync(s));
			//}

			///*********************************************/

			for (; i < data.size() - 1;)
			{
				const auto & blockIndex = data[i].Key();
				const auto posInCache = Vec3i(blockIndex.x*blockSize.x, blockIndex.y*blockSize.y, blockIndex.z*blockSize.z);
				GL(glTextureSubImage3D(textureId, 0, posInCache.x, posInCache.y, posInCache.z, blockSize.x, blockSize.y, blockSize.z, IF_RED, GL_UNSIGNED_BYTE, offset[curPBO]));
				GL(fences[curPBO] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0));
				i++;
				const auto nextPBO = i % pboCount;

				int is = 0;
				GL(is = glIsSync(fences[nextPBO]));
				if (is == GL_TRUE)
				{
					//GL(s = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0));
					GL(glClientWaitSync(fences[nextPBO], 0, 100000000));
					GL(glDeleteSync(fences[nextPBO]));
				}
				const auto d = cpuVolumeData->ReadBlockDataFromCPUCache(data[i].BlockId());
				memcpy(pboPtrs[nextPBO], d, blockBytes);
				curPBO = nextPBO;
			}
			const auto & blockIndex = data[i].Key();
			const auto posInCache = Vec3i(blockIndex.x*blockSize.x, blockIndex.y*blockSize.y, blockIndex.z*blockSize.z);
			GL(glTextureSubImage3D(textureId, 0, posInCache.x, posInCache.y, posInCache.z, blockSize.x, blockSize.y, blockSize.z, IF_RED, GL_UNSIGNED_BYTE, offset[curPBO]));
			GL(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0));


		}
		void OutOfCoreVolumeTexture::CreatePBOs(int bytes)
		{
			this->bytes = bytes;
			constexpr GLbitfield
				mapping_flags = GL_MAP_WRITE_BIT | GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT,
				storage_flags = GL_DYNAMIC_STORAGE_BIT | mapping_flags;
			GL(glCreateBuffers(1, &pbo));
			GL(glNamedBufferStorage(pbo, pboCount*bytes, nullptr, storage_flags));
			void * ptr = nullptr;
			GL(ptr = glMapNamedBufferRange(pbo, 0, pboCount*bytes, mapping_flags));
			for (int i = 0; i < pboCount; i++)
			{
				offset[i] = (void*)(i * bytes);
				pboPtrs[i] = (uint8_t*)ptr + i * bytes;
			}
		}
		void OutOfCoreVolumeTexture::DestroyPBOs()
		{
			GL(glUnmapNamedBuffer(pbo));
			GL(glDeleteBuffers(1, &pbo));
			pbo = 0;
		}
		Size3 OutOfCoreVolumeTexture::EvalTextureSize(const Size3 & hint) const
		{
			assert(cpuVolumeData);
			const auto blockSize = cpuVolumeData->BlockSize();
			//auto texSize = blockSize * Size3{ 8,8,8 };
			return hint * blockSize;
		}

		size_t OutOfCoreVolumeTexture::EvalIDBufferSize(const Size3& hint) const
		{
			return 5000;
		}

		OutOfCoreVolumeTexture::OutOfCoreVolumeTexture(const std::string& fileName)
		{

			// Open the volume data file

			cpuVolumeData = MakeRef<CPUVolumeDataCache>(fileName);

			auto texSetupParams = MakeRef<TexCreateParams>();
			const auto texSize = EvalTextureSize(cpuVolumeData->BlockDim());
			//std::cout << "Cache Texture Cache:" << texSize << std::endl;
			texSetupParams->SetSize(texSize.x, texSize.y, texSize.z);
			texSetupParams->SetTextureFormat(TF_R8);
			texSetupParams->SetTextureTarget(TD_TEXTURE_3D);

			volumeData = MakeRef<Texture>();		// gpu volume data cache size
			volumeData->SetSetupParams(texSetupParams);
			volumeData->CreateTexture();


			// Init PBO
			const auto blockBytes = cpuVolumeData->BlockSize().Prod() * sizeof(char);
			std::cout << blockBytes << std::endl;
			//for (int i = 0; i < 2; i++)
			//{
			//	pbo[i] = MakeRef<BufferObject>();
			//	pbo[i]->CreateBufferObject();
			//	pbo[i]->SetBufferData(blockBytes, nullptr, BU_STREAM_DRAW);
			//}
			CreatePBOs(blockBytes);

			const auto blocks = cpuVolumeData->BlockDim().Prod();
			const auto dim = cpuVolumeData->BlockDim();

			// Test:

			//Create a page table texture
			texSetupParams = MakeRef<TexCreateParams>();
			const auto mappingTableSize = cpuVolumeData->BlockDim();
			texSetupParams->SetSize(mappingTableSize.x, mappingTableSize.y, mappingTableSize.z);
			texSetupParams->SetTextureFormat(TF_RGBA32UI);
			texSetupParams->SetTextureTarget(TD_TEXTURE_3D);

			mappingTable = MakeRef<Texture>();		// gpu volume data cache size
			mappingTable->SetSetupParams(texSetupParams);
			mappingTable->CreateTexture();

			//std::cout << "Block Dim:" << cpuVolumeData->BlockDim() << std::endl;
			mappingTableManager = MakeRef<MappingTableManager>(cpuVolumeData->BlockDim());	// It need the virtual memory space size and the block size

			//Test:

			std::vector<VirtualMemoryBlockIndex> virtualSpaceAddress;
			for (int i = 0; i < blocks; i++)
			{
				virtualSpaceAddress.emplace_back(i, dim.x, dim.y, dim.z);
				const auto & b = virtualSpaceAddress.back();
			}

			const auto pa = mappingTableManager->UpdatePageTable(virtualSpaceAddress);

			std::vector<SubDataDescriptor> descs;

			descs.reserve(blocks);
			for (int i = 0; i < blocks; i++)
			{
				auto ptr = cpuVolumeData->ReadBlockDataFromCPUCache(i);
				const auto index3d = ysl::Dim(i, { dim.x,dim.y });
				descs.emplace_back((void*)ptr, PhysicalMemoryBlockIndex{ index3d.x,index3d.y,index3d.z }, i);
			}
			SetSubTextureDataUsePBO(descs);
			//

			// Create Atomic Buffer
			atomicCounterBuffer = MakeRef<BufferObject>(VM_BT_ATOMIC_COUNTER_BUFFER);
			int zero = 0;
			atomicCounterBuffer->SetLocalData(&zero, sizeof(int));
			atomicCounterBuffer->SetBufferData(BU_STREAM_READ, false);

			// Create Hash Table Buffer

			const auto idBufferSize = EvalIDBufferSize(cpuVolumeData->BlockDim());
			blockIdBuffer = MakeRef<BufferObject>(VM_BT_SHADER_STORAGE_BUFFER);
			blockIdBuffer->SetBufferData(idBufferSize * sizeof(int32_t), nullptr, BU_STREAM_READ);

			duplicateRemoveHash = MakeRef<BufferObject>(VM_BT_SHADER_STORAGE_BUFFER);
			std::vector<uint32_t> emptyBuffer(idBufferSize, 0);
			duplicateRemoveHash->SetLocalData(emptyBuffer.data(), emptyBuffer.size());
			duplicateRemoveHash->SetBufferData(BU_STREAM_READ, false);

		}

		void OutOfCoreVolumeTexture::OnDrawCallStart(OutOfCorePrimitive* p)
		{

		}

		void OutOfCoreVolumeTexture::OnDrawCallFinished(OutOfCorePrimitive* p)
		{
			//return;

			blockIdLocalBuffer.clear();
			auto aptr = atomicCounterBuffer->MapBuffer(BA_READ_WRITE);
			assert(aptr);
			int blocks = *((int*)aptr);
			*((int*)aptr) = 0;
			atomicCounterBuffer->UnmapBuffer();

			if (blocks == 0)
			{
				p->SetRenderFinished(true);
				return;
			}

			auto bptr = (int*)blockIdBuffer->MapBuffer(BA_READ_ONLY);
			blockIdLocalBuffer.resize(blocks);
			memcpy(blockIdLocalBuffer.data(), bptr, sizeof(int) * blocks);
			blockIdBuffer->UnmapBuffer();
			duplicateRemoveHash->SetBufferData(BU_STREAM_READ, false);		//Reset Hash Table


			const auto dim = cpuVolumeData->BlockDim();
			const auto physicalBlockCount = dim.Prod();
			std::vector<VirtualMemoryBlockIndex> virtualSpaceAddress;
			virtualSpaceAddress.reserve(blocks);

			std::vector<SubDataDescriptor> descs;
			descs.reserve(blocks);

			for (int i = 0; i < blocks && i < physicalBlockCount; i++)
			{
				virtualSpaceAddress.emplace_back(blockIdLocalBuffer[i], dim.x, dim.y, dim.z);
				//const auto & b = virtualSpaceAddress.back();
			}

			const auto pa = mappingTableManager->UpdatePageTable(virtualSpaceAddress);
			mappingTable->SetSubTextureData(mappingTableManager->GetData(), 0, 0, 0, dim.x, dim.y, dim.z);
			for(int i = 0;i < pa.size();i++)
			{
				descs.emplace_back(nullptr, PhysicalMemoryBlockIndex{ pa[i].x,pa[i].y,pa[i].z }, blockIdLocalBuffer[i]);
			}
			SetSubTextureDataUsePBO(descs);


		}

		void MappingTableManager::InitCPUPageTable(const Size3& blockDim)
		{
			// Only initialization flag filed, the table entry is determined by cache miss at run time using lazy evaluation policy.
			pageTable = Linear3DArray<PageTableEntry>(blockDim, nullptr);
			for (auto z = 0; z < pageTable.Size().z; z++)
				for (auto y = 0; y < pageTable.Size().y; y++)
					for (auto x = 0; x < pageTable.Size().x; x++)
					{
						PageTableEntry entry;
						entry.x = -1;
						entry.y = -1;
						entry.z = -1;
						entry.w = Unmapped;
						(pageTable)(x, y, z) = entry;
					}
		}

		void MappingTableManager::InitLRUList(const Size3& physicalMemoryBlock, const Size3& page3DSize)
		{
			for (auto z = 0; z < physicalMemoryBlock.z; z++)
				for (auto y = 0; y < physicalMemoryBlock.y; y++)
					for (auto x = 0; x < physicalMemoryBlock.x; x++)
					{
						g_lruList.emplace_back(
							PageTableEntryAbstractIndex(-1, -1, -1),
							PhysicalMemoryBlockIndex(x * page3DSize.x, y * page3DSize.y, z * page3DSize.z)
						);
					}
		}

		void MappingTableManager::InitLRUList(const Size3& physicalMemoryBlock)
		{
			for (auto z = 0; z < physicalMemoryBlock.z; z++)
				for (auto y = 0; y < physicalMemoryBlock.y; y++)
					for (auto x = 0; x < physicalMemoryBlock.x; x++)
					{
						g_lruList.emplace_back(
							PageTableEntryAbstractIndex(-1, -1, -1),
							PhysicalMemoryBlockIndex(x, y, z)
						);
					}
		}

		std::vector<PhysicalMemoryBlockIndex> MappingTableManager::UpdatePageTable(
			const std::vector<VirtualMemoryBlockIndex>& missedBlockIndices)
		{
			const auto missedBlocks = missedBlockIndices.size();

			std::vector<PhysicalMemoryBlockIndex> physicalIndices;
			physicalIndices.reserve(missedBlocks);
			// Update LRU List 
			for (int i = 0; i < missedBlocks; i++)
			{
				const auto& index = missedBlockIndices[i];
				auto& pageTableEntry = pageTable(index.x, index.y, index.z);
				auto& last = g_lruList.back();
				pageTableEntry.w = EntryFlag::Mapped; // Map the flag of page table entry
				// last.second is the cache block index

				const auto xInCache = last.second.x;
				const auto yInCache = last.second.y;
				const auto zInCache = last.second.z;

				pageTableEntry.x = xInCache; // fill the page table entry
				pageTableEntry.y = yInCache;
				pageTableEntry.z = zInCache;

				if (last.first.x != -1)
				{
					pageTable(last.first.x, last.first.y, last.first.z).w = EntryFlag::Unmapped;
				}

				// critical section : last
				last.first.x = index.x;
				last.first.y = index.y;
				last.first.z = index.z;
				//
				g_lruList.splice(g_lruList.begin(), g_lruList, --g_lruList.end()); // move from tail to head, LRU policy

				// critical section physicalIndex
				physicalIndices.push_back({ xInCache, yInCache, zInCache });
				//std::cout << last.first.x << " " << last.first.y << " " << last.first.z << " " << xInCache << " " << yInCache << " " << zInCache << std::endl;
			}

			return physicalIndices;
		}

	}
}

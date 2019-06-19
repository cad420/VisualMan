
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
		}

		void OutOfCoreVolumeTexture::SetSubTextureDataUsePBO(const std::vector<BlockDescriptor>& data)
		{
			// Ping-Pong PBO Transfer
			assert(volumeDataTexture[0]->Handle());

			const auto blockSize = cpuVolumeData->BlockSize();
			const auto textureId = volumeDataTexture[0]->Handle();
			const auto blockBytes = this->bytes;

			Timer t;
			t.begin();

			//auto curPBO = 0;
			//auto i = 0;
			//const auto dd = cpuVolumeData->ReadBlockDataFromCPUCache(data[i].Key());
			//GL(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbos->Handle()));
			//	auto pp = pboPtrs[curPBO];
			//memcpy(pp, dd, blockBytes);
			//for (; i < data.size() - 1;)
			//{
			//	const auto & blockIndex = data[i].Value();
			//	const auto posInCache = Vec3i(blockIndex.x*blockSize.x, blockIndex.y*blockSize.y, blockIndex.z*blockSize.z);
			//	GL(glTextureSubImage3D(textureId, 0, posInCache.x, posInCache.y, posInCache.z, blockSize.x, blockSize.y, blockSize.z, IF_RED, GL_UNSIGNED_BYTE, offset[curPBO]));
			//	GL(fences[curPBO] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0));
			//	i++;
			//	const auto nextPBO = i % pboCount;

			//	int is = 0;
			//	GL(is = glIsSync(fences[nextPBO]));
			//	if (is == GL_TRUE)
			//	{
			//		//GL(s = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0));
			//		GL(glClientWaitSync(fences[nextPBO], 0, 100000000));
			//		GL(glDeleteSync(fences[nextPBO]));
			//	}
			//	const auto d = cpuVolumeData->ReadBlockDataFromCPUCache(data[i].Key());
			//	memcpy(pboPtrs[nextPBO], d, blockBytes);
			//	curPBO = nextPBO;
			//}
			//const auto & blockIndex = data[i].Value();
			//const auto posInCache = Vec3i(blockIndex.x*blockSize.x, blockIndex.y*blockSize.y, blockIndex.z*blockSize.z);
			//GL(glTextureSubImage3D(textureId, 0, posInCache.x, posInCache.y, posInCache.z, blockSize.x, blockSize.y, blockSize.z, IF_RED, GL_UNSIGNED_BYTE, offset[curPBO]));
			//GL(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0));


			for (int i = 0; i < data.size(); i++)
			{
				const auto posInCache = Vec3i(blockSize) * data[i].Value().ToVec3i();
				GL(glTextureSubImage3D(volumeDataTexture[data[i].Value().GetPhysicalStorageUnit()]->Handle(), 0, posInCache.x, posInCache.y, posInCache.z, blockSize.x, blockSize.y, blockSize.z, IF_RED, IT_UNSIGNED_BYTE, cpuVolumeData->ReadBlockDataFromCPUCache(data[i].Key())));
			}

			t.end();
			totalBlocks += data.size();
			time += t.to_seconds();

		}
		void OutOfCoreVolumeTexture::CreatePBOs(int bytes)
		{
			this->bytes = bytes;
			pbos = MakeRef<BufferObject>();
			pbos->CreateImmutableBufferObject(bytes*pboCount, nullptr, storage_flags);
			pbos->MapBufferRange(0, pboCount*bytes, mapping_flags);
			for (int i = 0; i < pboCount; i++)
			{
				offset[i] = (void*)(i * bytes);
				pboPtrs[i] = pbos->MappedPointer<char*>() + i * bytes;
			}
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
			return 40000;
		}

		OutOfCoreVolumeTexture::OutOfCoreVolumeTexture(const std::string& fileName)
		{

			// Open the volume data file
			cpuVolumeData = MakeRef<CPUVolumeDataCache>(fileName,Size3{32,32,32});

			auto texSetupParams = MakeRef<TexCreateParams>();
			const auto l = cpuVolumeData->BlockDim().x;
			const auto cacheVolumeSizeHint = Size3{ 12,12,12 };
			const auto texSize = EvalTextureSize(cacheVolumeSizeHint);

			//std::cout << "Cache Texture Cache:" << texSize << std::endl;

			texSetupParams->SetSize(texSize.x, texSize.y, texSize.z);
			texSetupParams->SetTextureFormat(TF_R8);
			texSetupParams->SetTextureTarget(TD_TEXTURE_3D);

			volumeDataTexture[0] = MakeRef<Texture>();		// gpu volume data cache size
			volumeDataTexture[0]->SetSetupParams(texSetupParams);
			volumeDataTexture[0]->CreateTexture();

			volumeDataTexture[1] = MakeRef<Texture>();
			volumeDataTexture[1]->SetSetupParams(texSetupParams);
			volumeDataTexture[1]->CreateTexture();

			volumeDataTexture[2] = MakeRef<Texture>();
			volumeDataTexture[2]->SetSetupParams(texSetupParams);
			volumeDataTexture[2]->CreateTexture();

			volumeDataTexture[3] = MakeRef<Texture>();
			volumeDataTexture[3]->SetSetupParams(texSetupParams);
			volumeDataTexture[3]->CreateTexture();

			std::cout << cpuVolumeData->BlockDim() << std::endl;
			std::cout << cpuVolumeData->BlockSize() << std::endl;

			std::cout << cpuVolumeData->CPUCacheBlockSize() << std::endl;
			std::cout << cpuVolumeData->CPUCacheSize() << std::endl;
			std::cout << cpuVolumeData->OriginalDataSize() << std::endl;
			std::cout << cpuVolumeData->Padding() << std::endl;

			// Init PBO
			const auto blockBytes = cpuVolumeData->BlockSize().Prod() * sizeof(char);
			CreatePBOs(blockBytes);

			const auto blocks = cpuVolumeData->BlockDim().Prod();
			const auto dim = cpuVolumeData->BlockDim();

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
			mappingTableManager = MakeRef<MappingTableManager>(cpuVolumeData->BlockDim(), cacheVolumeSizeHint,3);	// It need the virtual memory space size and the block size

			mappingTable->SetSubTextureData(mappingTableManager->GetData(), 0, 0, 0, mappingTableSize.x, mappingTableSize.y, mappingTableSize.z);

			// Create Atomic Buffer
			atomicCounterBuffer = MakeRef<BufferObject>(VM_BT_ATOMIC_COUNTER_BUFFER);
			int zero = 0;
			atomicCounterBuffer->SetLocalData(&zero, sizeof(int));
			//atomicCounterBuffer->ReallocBufferData(BU_STREAM_READ, false);
			atomicCounterBuffer->CreateImmutableBufferObject(4, nullptr, storage_flags);
			atomicCounterBuffer->SetBufferSubDataFromLocalSubData(0, 0, 4);
			atomicCounterBuffer->MapBufferRange(0, 4, mapping_flags);

			// Create Hash Table Buffer

			const auto idBufferSize = EvalIDBufferSize(cpuVolumeData->BlockDim());
			blockIdBuffer = MakeRef<BufferObject>(VM_BT_SHADER_STORAGE_BUFFER);
			//blockIdBuffer->ReallocBufferData(idBufferSize * sizeof(int32_t), nullptr, BU_STREAM_READ);
			blockIdBuffer->CreateImmutableBufferObject(idBufferSize * sizeof(int32_t), nullptr, storage_flags);

			blockIdBuffer->MapBufferRange(0, idBufferSize * sizeof(int32_t), mapping_flags);

			duplicateRemoveHash = MakeRef<BufferObject>(VM_BT_SHADER_STORAGE_BUFFER);
			std::vector<uint32_t> emptyBuffer(idBufferSize * sizeof(int32_t), 0);
			duplicateRemoveHash->SetLocalData(emptyBuffer.data(), emptyBuffer.size());
			duplicateRemoveHash->CreateImmutableBufferObject(idBufferSize * sizeof(int32_t), nullptr, storage_flags);
			duplicateRemoveHash->MapBufferRange(0, idBufferSize * sizeof(int32_t), mapping_flags);
			//duplicateRemoveHash->ReallocBufferData(BU_STREAM_READ, false);
		}

		void OutOfCoreVolumeTexture::OnDrawCallStart(OutOfCorePrimitive* p)
		{

		}

		void OutOfCoreVolumeTexture::OnDrawCallFinished(OutOfCorePrimitive* p)
		{
			//int times[10];
			glFinish();		// wait the memory to be done

			blockIdLocalBuffer.clear();
			const auto counter = (atomicCounterBuffer->MappedPointer<int*>());
			const auto blocks = *counter;
			*(counter) = 0;
			//std::cout << blocks << std::endl;
			if (blocks == 0)  // render finished
			{
				p->SetRenderFinished(true);
				Log("Time:%f, BandWidth:%.2fGb/s, blocks:%d", time, totalBlocks*2.0 / 1024.0 / time, totalBlocks);
				time = 0;
				totalBlocks = 0;
				return;
			}

			blockIdLocalBuffer.resize(blocks);

			memcpy(blockIdLocalBuffer.data(), blockIdBuffer->MappedPointer<int*>(), sizeof(int) * blocks);
			memset(duplicateRemoveHash->MappedPointer<void*>(), 0, duplicateRemoveHash->BufferObjectSize()); // reset hash

			const auto dim = cpuVolumeData->BlockDim();
			const auto physicalBlockCount = dim.Prod();
			std::vector<VirtualMemoryBlockIndex> virtualSpaceAddress;
			virtualSpaceAddress.reserve(blocks);
			std::vector<BlockDescriptor> descs;
			descs.reserve(blocks);

			for (int i = 0; i < blocks && i < physicalBlockCount; i++)
			{
				virtualSpaceAddress.emplace_back(blockIdLocalBuffer[i], dim.x, dim.y, dim.z);
			}

			const auto physicalSpaceAddress = mappingTableManager->UpdatePageTable(virtualSpaceAddress);

			for (int i = 0; i < physicalSpaceAddress.size(); i++)
			{
				descs.emplace_back(physicalSpaceAddress[i], virtualSpaceAddress[i]);
			}

			SetSubTextureDataUsePBO(descs);			// Upload missed blocks
			mappingTable->SetSubTextureData(mappingTableManager->GetData(), 0, 0, 0, dim.x, dim.y, dim.z);
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
						entry.SetMapFlag(EM_UNMAPPED);
						//entry.w = EM_UNMAPPED;
						(pageTable)(x, y, z) = entry;
					}
		}

		//void MappingTableManager::InitLRUList(const Size3& physicalMemoryBlock, const Size3& page3DSize)
		//{
		//	for (auto z = 0; z < physicalMemoryBlock.z; z++)
		//		for (auto y = 0; y < physicalMemoryBlock.y; y++)
		//			for (auto x = 0; x < physicalMemoryBlock.x; x++)
		//			{
		//				g_lruList.emplace_back(
		//					PageTableEntryAbstractIndex(-1, -1, -1),
		//					PhysicalMemoryBlockIndex(x * page3DSize.x, y * page3DSize.y, z * page3DSize.z)
		//				);
		//			}
		//}

		void MappingTableManager::InitLRUList(const Size3& physicalMemoryBlock, int unitCount)
		{
			for (int i = 0; i < unitCount; i++)
				for (auto z = 0; z < physicalMemoryBlock.z; z++)
					for (auto y = 0; y < physicalMemoryBlock.y; y++)
						for (auto x = 0; x < physicalMemoryBlock.x; x++)
						{
							g_lruList.emplace_back
							(
								PageTableEntryAbstractIndex(-1, -1, -1),
								PhysicalMemoryBlockIndex(x, y, z, i)
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
				//pageTableEntry.w = EntryMapFlag::EM_MAPPED; // Map the flag of page table entry
				pageTableEntry.SetMapFlag(EM_MAPPED);
				// last.second is the cache block index

				physicalIndices.push_back(last.second);

				pageTableEntry.x = last.second.x; // fill the page table entry
				pageTableEntry.y = last.second.y;
				pageTableEntry.z = last.second.z;

				pageTableEntry.SetTextureUnit(last.second.GetPhysicalStorageUnit());
				//std::cout << "Get:" << pageTableEntry.GetTextureUnit() << std::endl;

				if (last.first.x != -1)
				{
					pageTable(last.first.x, last.first.y, last.first.z).SetMapFlag(EM_UNMAPPED);
				}
				// critical section : last
				last.first.x = index.x;
				last.first.y = index.y;
				last.first.z = index.z;
				//
				g_lruList.splice(g_lruList.begin(), g_lruList, --g_lruList.end()); // move from tail to head, LRU policy
			}

			return physicalIndices;
		}

	}
}

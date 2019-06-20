
#include "ooctexture.h"
#include <GL/gl3w.h>
#include <cstring>
#include "timer.h"

//#include "../lvdrenderer/virtualvolumehierachy.h"

namespace ysl
{
	namespace vm
	{
		void OutOfCoreVolumeTexture::PrintVideoMemoryUsageInfo()
		{

			size_t volumeTextureMemoryUsage = memoryEvalator->EvalPhysicalTextureSize().Prod()*memoryEvalator->EvalPhysicalTextureCount();
			size_t pageTableTextureMemoryUsage = cpuVolumeData->BlockDim().Prod() * sizeof(MappingTableManager::PageTableEntry);

			std::cout << "--------------- LVD Data Info---------------\n";
			std::cout << "Data Block Dimension: " << cpuVolumeData->BlockDim() << std::endl;
			std::cout << "Data Block Size: " << cpuVolumeData->BlockSize() << std::endl;
			std::cout << "Data Block Dimension In Host Memory: " << cpuVolumeData->CPUCacheBlockSize() << std::endl;
			std::cout << "Data Block Storage In Host Memory: " << cpuVolumeData->CPUCacheSize() << std::endl;
			std::cout << "Data Dimension Without Padding: " << cpuVolumeData->OriginalDataSize() << std::endl;
			std::cout << "Block Padding: " << cpuVolumeData->Padding() << std::endl;
			Log("------------Video Memory Usage ---------------");
			Log("Volume Texture Memory Usage: %d Bytes = %.2fMB", volumeTextureMemoryUsage, volumeTextureMemoryUsage*1.0 / 1024 / 1024);
			Log("Page Table Memory Usage: %d Bytes = %2.f MB", pageTableTextureMemoryUsage, pageTableTextureMemoryUsage*1.0 / 1024 / 1024);
			Log("ID Buffer Block Memory Usage: %d Bytes = %2.f MB", blockIdBuffer->BufferObjectSize(), blockIdBuffer->BufferObjectSize()*1.0 / 1024 / 1024);
			Log("Hash Buffer Block Memory Usage: %d Bytes = %2.f MB", hashBuffer->BufferObjectSize(), hashBuffer->BufferObjectSize()*1.0 / 1024 / 1024);
			Log("--------------End-----------------------------");
		}

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

		void OutOfCoreVolumeTexture::InitVolumeTextures()
		{
			auto texSetupParams = MakeRef<TexCreateParams>();
			const auto texSize = memoryEvalator->EvalPhysicalTextureSize();

			texSetupParams->SetSize(texSize.x, texSize.y, texSize.z);
			texSetupParams->SetTextureFormat(TF_R8);
			texSetupParams->SetTextureTarget(TD_TEXTURE_3D);

			volumeTexSamplerUBO = MakeRef<BufferObject>();
			volumeTexSamplerUBO->CreateImmutableBufferObject(4 * 4, nullptr, storage_flags);
			std::vector<int> texUnits;

			for (int i = 0; i < memoryEvalator->EvalPhysicalTextureCount(); i++)
			{
				auto vTex = MakeRef<Texture>();		// gpu volume data cache size
				vTex->SetSetupParams(texSetupParams);
				vTex->CreateTexture();
				volumeDataTexture.push_back(vTex);
				texUnits.push_back(i);
			}
			const int uboSize = texUnits.size() * sizeof(int);
			const auto ptr = volumeTexSamplerUBO->MapBufferRange(0, uboSize, mapping_flags);
			memcpy(ptr, texUnits.data(), uboSize);
		}

		void OutOfCoreVolumeTexture::InitMappingTable()
		{
			//Create a page table texture
			auto texSetupParams = MakeRef<TexCreateParams>();
			const auto mappingTableSize = cpuVolumeData->BlockDim();
			texSetupParams->SetSize(mappingTableSize.x, mappingTableSize.y, mappingTableSize.z);
			texSetupParams->SetTextureFormat(TF_RGBA32UI);
			texSetupParams->SetTextureTarget(TD_TEXTURE_3D);
			mappingTable = MakeRef<Texture>();		// gpu volume data cache size
			mappingTable->SetSetupParams(texSetupParams);
			mappingTable->CreateTexture();

			mappingTableManager = MakeRef<MappingTableManager>(cpuVolumeData->BlockDim(), memoryEvalator->EvalPhysicalBlockDim(), memoryEvalator->EvalPhysicalTextureCount());	// It need the virtual memory space size and the block size

			mappingTable->SetSubTextureData(mappingTableManager->GetData(), 0, 0, 0, mappingTableSize.x, mappingTableSize.y, mappingTableSize.z);

		}

		OutOfCoreVolumeTexture::OutOfCoreVolumeTexture(const std::string& fileName)
		{
			// Open the volume data file
			cpuVolumeData = MakeRef<CPUVolumeDataCache>(fileName, Size3{ 32,32,32 });

			if(cpuVolumeData->IsValid() == false)
			{
				throw std::runtime_error("can not load lvd data");
			}

			memoryEvalator = MakeRef<DefaultMemoryParamsEvaluator>(cpuVolumeData->BlockDim(), cpuVolumeData->BlockSize());

			InitVolumeTextures();
			InitMappingTable();

			// Create Atomic Buffer

			atomicCounterBuffer = MakeRef<BufferObject>(VM_BT_ATOMIC_COUNTER_BUFFER);
			int zero = 0;
			atomicCounterBuffer->SetLocalData(&zero, sizeof(int));
			atomicCounterBuffer->CreateImmutableBufferObject(4, nullptr, storage_flags);
			atomicCounterBuffer->SetBufferSubDataFromLocalSubData(0, 0, 4);
			atomicCounterBuffer->MapBufferRange(0, 4, mapping_flags);

			// Create Hash Table Buffer

			const auto idBufferBytes = memoryEvalator->EvalIDBufferCount() * sizeof(int32_t);
			blockIdBuffer = MakeRef<BufferObject>(VM_BT_SHADER_STORAGE_BUFFER);
			blockIdBuffer->CreateImmutableBufferObject(idBufferBytes, nullptr, storage_flags);
			blockIdBuffer->MapBufferRange(0, idBufferBytes, mapping_flags);


			const auto hashBufferBytes = memoryEvalator->EvalHashBufferSize() * sizeof(int);
			hashBuffer = MakeRef<BufferObject>(VM_BT_SHADER_STORAGE_BUFFER);
			std::vector<uint32_t> emptyBuffer(hashBufferBytes, 0);
			hashBuffer->SetLocalData(emptyBuffer.data(), emptyBuffer.size());
			hashBuffer->CreateImmutableBufferObject(hashBufferBytes, nullptr, storage_flags);
			hashBuffer->MapBufferRange(0, hashBufferBytes, mapping_flags);


			PrintVideoMemoryUsageInfo();



		}

		void OutOfCoreVolumeTexture::OnDrawCallStart(OutOfCorePrimitive* p)
		{

		}

		void OutOfCoreVolumeTexture::OnDrawCallFinished(OutOfCorePrimitive* p)
		{
			glFinish();		// wait the memory to be done
			blockIdLocalBuffer.clear();
			const auto counter = (atomicCounterBuffer->MappedPointer<int*>());
			const auto blocks = *counter;
			*(counter) = 0;
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
			memset(hashBuffer->MappedPointer<void*>(), 0, hashBuffer->BufferObjectSize()); // reset hash

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

		Size3 DefaultMemoryParamsEvaluator::EvalPhysicalTextureSize() const
		{
			return Size3{ 12,12,12 } *blockSize;
		}

		Size3 DefaultMemoryParamsEvaluator::EvalPhysicalBlockDim() const
		{
			return Size3{ 12,12,12 };
		}

		int DefaultMemoryParamsEvaluator::EvalPhysicalTextureCount() const
		{
			return 3;
		}

		int DefaultMemoryParamsEvaluator::EvalHashBufferSize() const
		{
			return virtualDim.Prod();
		}

		int DefaultMemoryParamsEvaluator::EvalIDBufferCount() const
		{
			return virtualDim.Prod();
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

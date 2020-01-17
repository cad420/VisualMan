
#include "ooctexture.h"
#include <VMUtils/timer.hpp>
#include <GL/gl3w.h>
#include <cstring>
#include <iostream>
#include <rapidjson/document.h>
#include <fstream>
#include <rapidjson/istreamwrapper.h>
#include <VMFoundation/pluginloader.h>
#include <rapidjson/pointer.h>
#include <VMUtils/vmnew.hpp>

namespace vm
{
void OutOfCoreVolumeTexture::PrintVideoMemoryUsageInfo( std::ostream &os )
{
	const size_t volumeTextureMemoryUsage = memoryEvaluators->EvalPhysicalTextureSize().Prod() * memoryEvaluators->EvalPhysicalTextureCount();
	size_t pageTableBufferBytes = 0;
	size_t totalCPUMemoryUsage = 0;

	for ( int i = 0; i < lodCount; i++ ) {
		fprintln( os, "===========LOD[{}]==============", i );
		fprintln( os, "Data Resolution: {}", cpuVolumeData[i]->DataSizeWithoutPadding());
		fprintln( os, "Block Dimension: {}", cpuVolumeData[ i ]->BlockDim() );
		fprintln( os, "Block Size: {}", cpuVolumeData[ i ]->BlockSize() );
		fprintln( os, "Data Size: {.2} GB", (cpuVolumeData[ i ]->BlockDim() * cpuVolumeData[i]->BlockSize()).Prod() * 1.0/1024/1024/1024 );
		fprintln( os, "CPU Memory Usage: {.2} GB", cpuVolumeData[ i ]->CPUCacheSize().Prod()*1.0/1024/1024/1024 );
		
		const auto blocks = cpuVolumeData[ i ]->BlockDim().Prod();
		fprintln( os, "Hash Memory Usage: {}, Offset: {}", blocks * sizeof( uint32_t ), lodInfo[ i ].hashBufferOffset );
		fprintln( os, "IDBuffer Memory Usage: {}, Offset: {}", blocks * sizeof( uint32_t ), lodInfo[ i ].idBufferOffset );
		fprintln( os, "PageTable Memory Usage: {}, Offset: {}", mappingTableManager->GetBytes( i ), lodInfo[ i ].pageTableOffset );

		pageTableBufferBytes += mappingTableManager->GetBytes( i );
		totalCPUMemoryUsage += cpuVolumeData[ i ]->CPUCacheSize().Prod();
	}

	const auto totalGPUMemoryUsage = pageTableBufferBytes + volumeTextureMemoryUsage + blockIdBuffer->BufferObjectSize() + hashBuffer->BufferObjectSize();

	println( "BlockDim: {} | Texture Size: {}", memoryEvaluators->EvalPhysicalBlockDim(), memoryEvaluators->EvalPhysicalTextureSize() );
	fprintln( os, "------------Summary Memory Usage ---------------" );
	fprintln( os, "Data Resolution: {}", cpuVolumeData[ 0 ]->DataSizeWithoutPadding() );
	fprintln( os, "Volume Texture Memory Usage: {} Bytes = {.2} MB", volumeTextureMemoryUsage, volumeTextureMemoryUsage * 1.0 / 1024 / 1024 );
	fprintln( os, "Page Table Memory Usage: {} Bytes = {.2} MB", pageTableBufferBytes, pageTableBufferBytes * 1.0 / 1024 / 1024 );
	fprintln( os, "Total ID Buffer Block Memory Usage: {} Bytes = {.2} MB", blockIdBuffer->BufferObjectSize(), blockIdBuffer->BufferObjectSize() * 1.0 / 1024 / 1024 );
	fprintln( os, "Total Hash Buffer Block Memory Usage: {} Bytes = {.2} MB", hashBuffer->BufferObjectSize(), hashBuffer->BufferObjectSize() * 1.0 / 1024 / 1024 );
	fprintln( os, "Total Volume Data GPU Memory Usage: {} Bytes = {.2} GB",totalGPUMemoryUsage, totalGPUMemoryUsage*1.0/1024/1024/1024 );
	fprintln( os, "Total CPU Memory Usage: {} Bytes = {.2} GB", totalCPUMemoryUsage, totalCPUMemoryUsage * 1.0 / 1024 / 1024 / 1024 );
	fprintln( os, "================================" );

}

void OutOfCoreVolumeTexture::PrintBlockResidenceInfo( std::ostream &os )
{
	size_t t = 0;
	for ( int i = 0; i < lodCount; i++ ) {
		fprint(os, "LOD{}:{} | ", i, mappingTableManager->GetResidentBlocks( i ) );
		t += mappingTableManager->GetResidentBlocks( i );
	}
	fprintln( os, "Total: {}", t );
}

void OutOfCoreVolumeTexture::BindToOutOfCorePrimitive( VMRef<OutOfCorePrimitive> oocPrimitive )
{
	if ( oocPrimitive ) {
		oocPrimitive->RemoveOutOfCoreResources( shared_from_this() );
		oocPrimitive->SetOutOfCoreResources( shared_from_this() );
	}
}

OutOfCoreVolumeTexture::~OutOfCoreVolumeTexture()
{
}

void OutOfCoreVolumeTexture::SetSubTextureDataUsePBO( const std::vector<BlockDescriptor> &data, int lod )
{
	// Ping-Pong PBO Transfer
	assert( volumeDataTexture[ 0 ]->Handle() );

	const auto blockSize = cpuVolumeData[ lod ]->BlockSize();

	//const auto textureId = volumeDataTexture[lod]->Handle();
	//const auto blockBytes = this->bytes;

	//::vm::Timer t;
	//t.start();

	for ( int i = 0; i < data.size(); i++ ) {
		const auto posInCache = Vec3i( blockSize ) * data[ i ].Value().ToVec3i();
		const auto d = cpuVolumeData[ lod ]->GetPage( data[ i ].Key() );
		const auto texHandle = volumeDataTexture[ data[ i ].Value().GetPhysicalStorageUnit() ]->Handle();
		GL( glTextureSubImage3D( texHandle, 0, posInCache.x, posInCache.y, posInCache.z, blockSize.x, blockSize.y, blockSize.z, IF_RED, IT_UNSIGNED_BYTE, d ) );
	}
	//t.stop();
	totalBlocks += data.size();
	//time += t.duration_to_seconds();
}

void OutOfCoreVolumeTexture::InitVolumeTextures()
{
	auto texSetupParams = MakeVMRef<TexCreateParams>();

	const auto texSize = memoryEvaluators->EvalPhysicalTextureSize();

	texSetupParams->SetSize( texSize.x, texSize.y, texSize.z );
	//ysl::Vec3i size(256*5,256*5,256*5);
	//texSetupParams->SetSize( size.x,size.y,size.z );
	texSetupParams->SetTextureFormat( TF_R8 );
	texSetupParams->SetTextureTarget( TD_TEXTURE_3D );

	for ( int i = 0; i < memoryEvaluators->EvalPhysicalTextureCount(); i++ ) {
		auto vTex = MakeVMRef<Texture>();  // gpu volume data cache size
		vTex->SetSetupParams( texSetupParams );
		vTex->CreateTexture();
		volumeDataTexture.push_back( vTex );
	}
}

LVDFileInfo GetLVDFileInfoFromJson( const std::string &fileName )
{
	using namespace rapidjson;

	std::ifstream ifs( fileName );
	rapidjson::IStreamWrapper isw( ifs );
	Document d;
	d.ParseStream( isw );

	LVDFileInfo lvdInfo;

	auto lvdIter = d.FindMember( "lvd" );
	if ( lvdIter == d.MemberEnd() || !lvdIter->value.IsObject() ) {
		std::cout << "an invalid lod json file\n";
		return lvdInfo;
	}

	const auto fileNamesIter = lvdIter->value.FindMember( "fileNames" );

	if ( fileNamesIter == lvdIter->value.MemberEnd() || lvdIter->value.IsArray() ) {
		return lvdInfo;
	}

	for ( auto &n : fileNamesIter->value.GetArray() ) {
		lvdInfo.fileNames.push_back( n.GetString() );
	}

	lvdInfo.samplingRate = GetValueByPointerWithDefault( d, "/lvd/samplingRate", 0.01 ).GetFloat();
	lvdInfo.spacing.x = GetValueByPointerWithDefault( d, "/lvd/spacing/0", 1.0 ).GetFloat();
	lvdInfo.spacing.y = GetValueByPointerWithDefault( d, "/lvd/spacing/1", 1.0 ).GetFloat();
	lvdInfo.spacing.z = GetValueByPointerWithDefault( d, "/lvd/spacing/2", 1.0 ).GetFloat();

	return lvdInfo;
}

struct MyEvaluator : IVideoMemoryParamsEvaluator
{
private:
	const Size3 virtualDim;
	const Size3 blockSize;
	const std::size_t videoMem;
	int textureUnitCount = 0;
	Size3 finalBlockDim = { 0, 0, 0 };

public:
	MyEvaluator( const Size3 &virtualDim, const Size3 &blockSize, std::size_t videoMemory ) :
	  virtualDim( virtualDim ),
	  blockSize( blockSize ),
	  videoMem( videoMemory )
	{
		// We assume that we can only use 3/4 of total video memory and there are 4 texture units at most

		textureUnitCount = 4;
		const auto maxBytesPerTexUnit = videoMemory * 3 / 4 / textureUnitCount;
		std::size_t d = 0;
		while ( ++d ) {
			const auto memory = d * d * d * blockSize.Prod();
			if ( memory >= maxBytesPerTexUnit )
				break;
		}
		d--;
		//while ( d > 10 )
		//{
		//	d /= 2;
		//	textureUnitCount++;
		//}
		finalBlockDim = Size3{ d, d, d };
	}

	Size3 EvalPhysicalTextureSize() const override
	{
		return blockSize * EvalPhysicalBlockDim();
	}
	Size3 EvalPhysicalBlockDim() const override
	{
		return finalBlockDim;
	}

	int EvalPhysicalTextureCount() const override
	{
		return textureUnitCount;
	}
	~MyEvaluator() = default;
};


OutOfCoreVolumeTexture::OutOfCoreVolumeTexture( const LVDFileInfo &fileInfo, std::size_t videoMemory )
{
	// Open the volume data file
	lodCount = fileInfo.fileNames.size();
	cpuVolumeData.resize( lodCount );
	lodInfo.resize( lodCount );

	for ( int i = 0; i < lodCount; i++ ) {
		//cpuVolumeData[ i ] = MakeVMRef<MemoryPageAdapter>( fileInfo.fileNames[ i ] );

		const auto cap = fileInfo.fileNames[ i ].substr( fileInfo.fileNames[ i ].find_last_of( '.' ) );
		auto p = PluginLoader::GetPluginLoader()->CreatePlugin<I3DBlockFilePluginInterface>( cap );

		p->Open( fileInfo.fileNames[ i ] );
		if ( !p ) {
			println( "Failed to load plugin to read {} file", cap );
			throw std::runtime_error( "Failed to load plugin" );
		}
		cpuVolumeData[ i ] = VM_NEW<Block3DCache>( p, []( I3DBlockFilePluginInterface * p ) 
			{
			
			const auto bytes = p->GetDataSizeWithoutPadding().Prod();
			const size_t th = 8 * 1024 * 1024 * size_t(1024);
			size_t d = 0;
			const auto pageSize = p->Get3DPageSize().Prod();
			if ( bytes < th ) 
			{
				while ( d * d * d * pageSize < bytes ) d++;
			}
			else 
			{
				while ( d * d * d * pageSize < th ) 
					d++;
			}
			
			return Size3{ d,d,d };
		    });
	}

	memoryEvaluators = MakeVMRef<MyEvaluator>( cpuVolumeData[ 0 ]->BlockDim(), cpuVolumeData[ 0 ]->BlockSize(), videoMemory );

	InitVolumeTextures();
	///[0] Gather buffer size before creating buffers

	size_t pageTableTotalEntries = 0;
	size_t hashBufferTotalBlocks = 0;
	size_t idBufferTotalBlocks = 0;
	std::vector<LODPageTableInfo> pageTableInfos;
	for ( int i = 0; i < cpuVolumeData.size(); i++ ) {
		LODPageTableInfo info;
		info.virtualSpaceSize = Vec3i( cpuVolumeData[ i ]->BlockDim() );
		info.offset = pageTableTotalEntries;
		pageTableInfos.push_back( info );

		lodInfo[ i ].volumeDataSizeNoRepeat = Vec4i( Vec3i(cpuVolumeData[ i ]->DataSizeWithoutPadding()) );
		const int padding = cpuVolumeData[ i ]->Padding();
		lodInfo[ i ].blockDataSizeNoRepeat = Vec4i(Vec3i( cpuVolumeData[ i ]->BlockSize() - Size3( 2 * padding, 2 * padding, 2 * padding ) ));
		lodInfo[ i ].pageTableSize = Vec4i(( info.virtualSpaceSize ));  // GLSL std140 layout
		lodInfo[ i ].pageTableOffset = pageTableTotalEntries;
		lodInfo[ i ].idBufferOffset = idBufferTotalBlocks;
		lodInfo[ i ].hashBufferOffset = hashBufferTotalBlocks;

		const auto blocks = cpuVolumeData[ i ]->BlockDim().Prod();
		pageTableTotalEntries += blocks;  // *sizeof(MappingTableManager::PageTableEntry);
		hashBufferTotalBlocks += blocks;  // *sizeof(uint32_t);
		idBufferTotalBlocks += blocks;	// *sizeof(uint32_t);
	}

	/// [1] Create Page Table Buffer and mapping table mananger
	pageTableBuffer = MakeVMRef<BufferObject>();
	pageTableBuffer->CreateImmutableBufferObject( pageTableTotalEntries * sizeof( MappingTableManager::PageTableEntry ), nullptr, storage_flags );
	const auto pageTablePtr = pageTableBuffer->MapBufferRange( 0, pageTableTotalEntries * sizeof( MappingTableManager::PageTableEntry ), mapping_flags );
	assert( pageTablePtr );

	for ( int i = 0; i < lodCount; i++ ) {
		pageTableInfos[ i ].external = (MappingTableManager::PageTableEntry *)pageTablePtr + pageTableInfos[ i ].offset;
	}

	mappingTableManager = MakeVMRef<MappingTableManager>( pageTableInfos,  // Create Mapping table for lods
														  memoryEvaluators->EvalPhysicalBlockDim(),
														  memoryEvaluators->EvalPhysicalTextureCount() );

	///[2] Create Atomic Buffer

	const size_t atomicBufferBytes = lodCount * sizeof( uint32_t );
	atomicCounterBuffer = MakeVMRef<BufferObject>( VM_BT_ATOMIC_COUNTER_BUFFER );
	std::vector<uint32_t> zeroBuffer( lodCount, 0 );
	atomicCounterBuffer->SetLocalData( zeroBuffer.data(), atomicBufferBytes );
	atomicCounterBuffer->CreateImmutableBufferObject( atomicBufferBytes, nullptr, storage_flags );
	atomicCounterBuffer->SetBufferSubDataFromLocalSubData( 0, 0, atomicBufferBytes );
	atomicCounterBuffer->MapBufferRange( 0, atomicBufferBytes, mapping_flags );

	///[3] Create ID Buffer

	//const auto idBufferBytes = memoryEvaluators->EvalIDBufferCount() * sizeof(int32_t);

	blockIdBuffer = MakeVMRef<BufferObject>( VM_BT_SHADER_STORAGE_BUFFER );
	blockIdBuffer->CreateImmutableBufferObject( idBufferTotalBlocks * sizeof( uint32_t ), nullptr, storage_flags );
	blockIdBuffer->MapBufferRange( 0, idBufferTotalBlocks, mapping_flags );

	///[4] Create Hash Buffer
	hashBuffer = MakeVMRef<BufferObject>( VM_BT_SHADER_STORAGE_BUFFER );
	std::vector<uint32_t> emptyBuffer( hashBufferTotalBlocks, 0 );
	hashBuffer->SetLocalData( emptyBuffer.data(), emptyBuffer.size() );
	hashBuffer->CreateImmutableBufferObject( hashBufferTotalBlocks * sizeof( uint32_t ), nullptr, storage_flags );
	hashBuffer->MapBufferRange( 0, hashBufferTotalBlocks, mapping_flags );

	///[5] Create LOD Info Buffer
	lodInfoBuffer = MakeVMRef<BufferObject>( VM_BT_SHADER_STORAGE_BUFFER );
	const auto lodInfoBytes = sizeof( _std140_layout_LODInfo ) * lodInfo.size();
	lodInfoBuffer->CreateImmutableBufferObject( lodInfoBytes, lodInfo.data(), storage_flags );

	PrintVideoMemoryUsageInfo(std::cout);
}

void OutOfCoreVolumeTexture::OnDrawCallStart( OutOfCorePrimitive *p )
{
}

void OutOfCoreVolumeTexture::OnDrawCallFinished( OutOfCorePrimitive *p )
{
	glFinish();  // wait the memory to be done
	bool render_finished = true;
	for ( int curLod = 0; curLod < lodCount; curLod++ ) {
		blockIdLocalBuffer.clear();
		const auto counter = ( atomicCounterBuffer->MappedPointer<int *>() );
		size_t blocks = *( counter + curLod );
		*( counter + curLod ) = 0;
		if ( blocks == 0 )  // render finished
			continue;
		blocks = ( std::min )( memoryEvaluators->EvalPhysicalBlockDim().Prod() * memoryEvaluators->EvalPhysicalTextureCount(), blocks );

		render_finished = false;

		blockIdLocalBuffer.resize( blocks );
		memcpy( blockIdLocalBuffer.data(), blockIdBuffer->MappedPointer<int *>() + lodInfo[ curLod ].idBufferOffset, sizeof( int ) * blocks );
		memset( hashBuffer->MappedPointer<void *>(), 0, hashBuffer->BufferObjectSize() );  // reset hash

		const auto dim = cpuVolumeData[ curLod ]->BlockDim();

		const auto physicalBlockCount = dim.Prod();

		std::vector<VirtualMemoryBlockIndex> virtualSpaceAddress;
		virtualSpaceAddress.reserve( blocks );
		std::vector<BlockDescriptor> descs;
		descs.reserve( blocks );
		//println( "lod: {}, blocks: {}", curLod, blocks );
		for ( int i = 0; i < blocks && i < physicalBlockCount; i++ ) {
			virtualSpaceAddress.emplace_back( blockIdLocalBuffer[ i ], dim.x, dim.y, dim.z );
		}

		const auto physicalSpaceAddress = mappingTableManager->UpdatePageTable( curLod, virtualSpaceAddress );

		for ( int i = 0; i < physicalSpaceAddress.size(); i++ ) {
			descs.emplace_back( physicalSpaceAddress[ i ], virtualSpaceAddress[ i ] );
		}

		SetSubTextureDataUsePBO( descs, curLod );  // Upload missed blocks
	}
	if ( render_finished ) {
		p->SetRenderFinished( render_finished );
		printf( "\r" );
		for ( int i = 0; i < lodCount; i++ )
			printf( "LOD%d:%d|", i, mappingTableManager->GetResidentBlocks( i ) );
		//Log("Time:{}, BandWidth:{.2}Gb/s, blocks:{}", time, totalBlocks*2.0 / 1024.0 / time, totalBlocks);
		time = 0;
		totalBlocks = 0;
	}
}

//DefaultMemoryParamsEvaluator::DefaultMemoryParamsEvaluator( const Size3 &virtualDim, const Size3 &blockSize,
//															std::size_t videoMemory ) :
//  virtualDim( virtualDim ),
//  blockSize( blockSize ),
//  videoMem( videoMemory )
//{
//	//if(videoMem ==0 )
//	//{
//	//	ysl::Error("No enough video memory");
//	//}
//	std::size_t d = 0;
//	textureUnitCount = 1;
//	while ( ++d ) {
//		const auto memory = d * d * d * blockSize.Prod();
//		if ( memory >= videoMem * 1024 )
//			break;
//	}
//	while ( d > 10 ) {
//		d /= 2;
//		textureUnitCount++;
//	}
//	finalBlockDim = Size3{ d, d, d };
//}
//
//Size3 DefaultMemoryParamsEvaluator::EvalPhysicalTextureSize() const
//{
//	return blockSize * EvalPhysicalBlockDim();
//}
//
//Size3 DefaultMemoryParamsEvaluator::EvalPhysicalBlockDim() const
//{
//	return { 10, 10, 10 };
//}
//
//int DefaultMemoryParamsEvaluator::EvalPhysicalTextureCount() const
//{
//	return 3;
//}
//
//void MappingTableManager::InitCPUPageTable( const Size3 &blockDim, void *external )
//{
//	// Only initialization flag filed, the table entry is determined by cache miss at run time using lazy evaluation policy.
//	if ( external == nullptr )
//		pageTable = Linear3DArray<PageTableEntry>( blockDim, nullptr );
//	else
//		pageTable = Linear3DArray<PageTableEntry>( blockDim.x, blockDim.y, blockDim.z, (PageTableEntry *)external, false );
//	size_t blockId = 0;
//	for ( auto z = 0; z < pageTable.Size().z; z++ )
//		for ( auto y = 0; y < pageTable.Size().y; y++ )
//			for ( auto x = 0; x < pageTable.Size().x; x++ ) {
//				PageTableEntry entry;
//				entry.x = -1;
//				entry.y = -1;
//				entry.z = -1;
//				entry.SetMapFlag( EM_UNMAPPED );
//				//entry.w = EM_UNMAPPED;
//				( pageTable )( x, y, z ) = entry;
//				lruMap[ blockId++ ] = lruList.end();
//			}
//}
//
//void MappingTableManager::InitLRUList( const Size3 &physicalMemoryBlock, int unitCount )
//{
//	for ( int i = 0; i < unitCount; i++ )
//		for ( auto z = 0; z < physicalMemoryBlock.z; z++ )
//			for ( auto y = 0; y < physicalMemoryBlock.y; y++ )
//				for ( auto x = 0; x < physicalMemoryBlock.x; x++ ) {
//					lruList.emplace_back(
//					  PageTableEntryAbstractIndex( -1, -1, -1 ),
//					  PhysicalMemoryBlockIndex( x, y, z, i ) );
//				}
//}
//
//MappingTableManager::MappingTableManager( const Size3 &virtualSpaceSize, const Size3 &physicalSpaceSize )
//{
//	InitCPUPageTable( virtualSpaceSize, nullptr );
//	InitLRUList( physicalSpaceSize, 1 );
//}
//
//MappingTableManager::MappingTableManager( const Size3 &virtualSpaceSize, const Size3 &physicalSpaceSize,
//										  int physicalSpaceCount )
//{
//	InitCPUPageTable( virtualSpaceSize, nullptr );
//	InitLRUList( physicalSpaceSize, physicalSpaceCount );
//}
//
//MappingTableManager::MappingTableManager( const Size3 &virtualSpaceSize, const Size3 &physicalSpaceSize,
//										  int physicalSpaceCount, void *external )
//{
//	assert( external );
//	InitCPUPageTable( virtualSpaceSize, external );
//	InitLRUList( physicalSpaceSize, physicalSpaceCount );
//}
//
//MappingTableManager::MappingTableManager( const std::vector<LODPageTableInfo> &infos, const Size3 &physicalSpaceSize, int physicalSpaceCount )
//{
//	const int lod = infos.size();
//	lodPageTables.resize( lod );
//	blocks.resize( lod );
//
//	// lod page table
//	for ( int i = 0; i < lod; i++ ) {
//		if ( infos[ i ].external == nullptr )
//			lodPageTables[ i ] = Linear3DArray<PageTableEntry>( Size3( infos[ i ].virtualSpaceSize ), nullptr );
//		else
//			lodPageTables[ i ] = Linear3DArray<PageTableEntry>( infos[ i ].virtualSpaceSize.x, infos[ i ].virtualSpaceSize.y, infos[ i ].virtualSpaceSize.z, (PageTableEntry *)infos[ i ].external, false );
//		size_t blockId = 0;
//
//		for ( auto z = 0; z < lodPageTables[ i ].Size().z; z++ )
//			for ( auto y = 0; y < lodPageTables[ i ].Size().y; y++ )
//				for ( auto x = 0; x < lodPageTables[ i ].Size().x; x++ ) {
//					PageTableEntry entry;
//					entry.x = -1;
//					entry.y = -1;
//					entry.z = -1;
//					entry.SetMapFlag( EM_UNMAPPED );
//					//entry.w = EM_UNMAPPED;
//					( lodPageTables[ i ] )( x, y, z ) = entry;
//
//					lruMap[ blockId++ ] = lruList.end();
//				}
//	}
//
//	// lod lru list
//
//	for ( int i = 0; i < physicalSpaceCount; i++ )
//		for ( auto z = 0; z < physicalSpaceSize.z; z++ )
//			for ( auto y = 0; y < physicalSpaceSize.y; y++ )
//				for ( auto x = 0; x < physicalSpaceSize.x; x++ ) {
//					lruList.emplace_back(
//					  PageTableEntryAbstractIndex( -1, -1, -1 ),
//					  PhysicalMemoryBlockIndex( x, y, z, i ) );
//				}
//}
//
//std::vector<PhysicalMemoryBlockIndex> MappingTableManager::UpdatePageTable( int lod,
//																			const std::vector<VirtualMemoryBlockIndex> &missedBlockIndices )
//{
//	const auto missedBlocks = missedBlockIndices.size();
//	std::vector<PhysicalMemoryBlockIndex> physicalIndices;
//	physicalIndices.reserve( missedBlocks );
//	// Update LRU List
//	for ( int i = 0; i < missedBlocks; i++ ) {
//		const auto &index = missedBlockIndices[ i ];
//		auto &pageTableEntry = lodPageTables[ lod ]( index.x, index.y, index.z );
//		const size_t flatBlockID = index.z * lodPageTables[ lod ].Size().x * lodPageTables[ lod ].Size().y + index.y * lodPageTables[ lod ].Size().x + index.x;
//		if ( pageTableEntry.GetMapFlag() == EM_MAPPED ) {
//			// move the already mapped node to the head
//			lruList.splice( lruList.begin(), lruList, lruMap[ flatBlockID ] );
//
//		} else {
//			auto &last = lruList.back();
//			//pageTableEntry.w = EntryMapFlag::EM_MAPPED; // Map the flag of page table entry
//			pageTableEntry.SetMapFlag( EM_MAPPED );
//			// last.second is the cache block index
//			physicalIndices.push_back( last.second );
//			pageTableEntry.x = last.second.x;  // fill the page table entry
//			pageTableEntry.y = last.second.y;
//			pageTableEntry.z = last.second.z;
//			pageTableEntry.SetTextureUnit( last.second.GetPhysicalStorageUnit() );
//			if ( last.first.x != -1 )  // detach previous mapped storage
//			{
//				lodPageTables[ last.first.lod ]( last.first.x, last.first.y, last.first.z ).SetMapFlag( EM_UNMAPPED );
//				lruMap[ flatBlockID ] = lruList.end();
//				blocks[ last.first.lod ]--;
//			}
//			// critical section : last
//			last.first.x = index.x;
//			last.first.y = index.y;
//			last.first.z = index.z;
//
//			last.first.lod = lod;  //
//
//			lruList.splice( lruList.begin(), lruList, --lruList.end() );  // move from tail to head, LRU policy
//			lruMap[ flatBlockID ] = lruList.begin();
//			blocks[ lod ]++;
//		}
//	}
//	return physicalIndices;
//}

}  // namespace vm

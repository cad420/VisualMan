
#include "lz4filereader.h"

#include <lz4.h>

#include <pluginloader.h>

namespace ysl
{
	
	IMPLEMENT_RTTI_NoConstructor(LZ4FileReader,I3DBlockFilePluginInterface)
}

void ysl::LZ4FileReader::Open(const std::string& fileName)
{
#ifdef _WIN32
	fileMapping = PluginLoader::GetPluginLoader()->CreatePlugin<IFileMappingPluginInterface>("windows");
#else
	fileMapping = PluginLoader::GetPluginLoader()->CreatePlugin<IFileMappingPluginInterface>("linux");
#endif

	if(fileMapping == nullptr)
	{
		throw std::runtime_error("failed to load file mapping plugin");
	}

	std::ifstream fileSize(fileName);
	if(fileSize.is_open() == false)
	{
		throw std::runtime_error("can not file file");
	}

	fileSize.seekg(0, std::ios::end);
	const size_t fileBytes = fileSize.tellg();
	fileSize.close();

	fileMapping->Open(fileName,fileBytes,FileAccess::Read,MapAccess::ReadOnly );

	dataPtr = (char *)fileMapping->FileMemPointer(0, fileBytes);

	size_t curOffset = 0;

	std::vector<int> ttt(10);

	// Get block bytes and offsets
	std::unique_ptr<char[]> buf(new char[header.HeaderSize()]);

	memcpy(buf.get(), dataPtr + curOffset, header.HeaderSize());
	curOffset += header.HeaderSize();
	header.Decode((unsigned char*)buf.get());
	
	int blockCount = 0;
	memcpy(&blockCount, dataPtr+curOffset, sizeof(int));
	curOffset += sizeof(int);
	//inFile.read((char*)&blockCount, sizeof(int));

	blockBytes = new std::vector<uint32_t>(blockCount);
	blockOffset = new std::vector<uint64_t>(blockCount);
	
	
	blockBytes->resize(blockCount);
	blockOffset->resize(blockCount);
	//inFile.read((char*)blockOffset.data(), sizeof(uint64_t) * blockCount);
	//inFile.read((char*)blockBytes.data(), sizeof(uint32_t) * blockCount);

	memcpy((char*)blockOffset->data(), dataPtr+ curOffset, sizeof(uint64_t) * blockCount);
	curOffset += sizeof(uint64_t) * blockCount;
	memcpy((char*)blockBytes->data(), dataPtr+ curOffset, sizeof(uint32_t) * blockCount);
	curOffset += sizeof(uint32_t) * blockCount;
	

	const auto blockLength = 1 << header.blockLengthInLog;
	blockBuf = new char[blockLength * blockLength * blockLength];
	compressedBuf = new char[blockLength * blockLength * blockLength];


	const size_t aBlockSize = blockLength;
	const auto vx = header.dataDim[0];
	const auto vy = header.dataDim[1];
	const auto vz = header.dataDim[2];
	
	const int bx = ((vx + aBlockSize - 1) & ~(aBlockSize - 1)) / aBlockSize;
	const int by = ((vy + aBlockSize - 1) & ~(aBlockSize - 1)) / aBlockSize;
	const int bz = ((vz + aBlockSize - 1) & ~(aBlockSize - 1)) / aBlockSize;

	const auto originalWidth = header.originalDataDim[0];
	const auto originalHeight = header.originalDataDim[1];
	const auto originalDepth = header.originalDataDim[2];
	
	vSize = ysl::Size3((vx), (vy), (vz));
	bSize = ysl::Size3(bx, by, bz);
	oSize = ysl::Size3(originalWidth, originalHeight, originalDepth);

	blockSize = Size3(blockLength,blockLength,blockLength);
	blockSizeBytes = blockSize.Prod();

}

int ysl::LZ4FileReader::GetPadding() const
{
	return header.padding;
}

ysl::Size3 ysl::LZ4FileReader::GetDataSizeWithoutPadding() const
{
	return oSize;
}

ysl::Size3 ysl::LZ4FileReader::Get3DPageSize() const
{
	return blockSize;
}

ysl::Size3 ysl::LZ4FileReader::Get3DPageCount() const
{
	return bSize;
}

int ysl::LZ4FileReader::Get3DPageSizeInLog() const
{
	return header.blockLengthInLog;
}

const void* ysl::LZ4FileReader::GetPage(size_t pageID)
{
	LZ4_decompress_safe(dataPtr+(*blockOffset)[pageID], blockBuf, (*blockBytes)[pageID], blockSizeBytes);
	return blockBuf;
}

size_t ysl::LZ4FileReader::GetPageSize() const
{
	return 1 << header.blockLengthInLog; // ?????
}

size_t ysl::LZ4FileReader::GetPhysicalPageCount() const
{
	return bSize.Prod();
}

size_t ysl::LZ4FileReader::GetVirtualPageCount() const
{
	return bSize.Prod();
}

ysl::LZ4FileReader::~LZ4FileReader()
{
	delete blockBytes;
	delete blockOffset;
	delete compressedBuf;
	delete blockBuf;
}

std::unique_ptr<ysl::Object> ysl::LZ4FileReaderFactory::Create(const std::string& key)
{
	return std::make_unique<LZ4FileReader>();
}

std::vector<std::string> ysl::LZ4FileReaderFactory::Keys() const
{
	return { ".lz4" };
}

EXPORT_PLUGIN_FACTORY_IMPLEMENT(ysl::LZ4FileReaderFactory)

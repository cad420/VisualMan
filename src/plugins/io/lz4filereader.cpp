
#include "lz4filereader.h"
#include <lz4.h>

namespace ysl
{
	IMPLEMENT_RTTI_NoParent(LZ4FileReader)
}

void ysl::LZ4FileReader::Open(const std::string& fileName)
{
	inFile.open(fileName, std::ios::binary);
	if (inFile.is_open() == false)
	{
		throw std::runtime_error("failed to load .lz4 file");
	}


	// Get block bytes and offsets

	std::unique_ptr<char[]> buf(new char[header.HeaderSize()]);
	inFile.read(buf.get(), header.HeaderSize());
	int blockCount = 0;
	inFile.read((char*)& blockCount, sizeof(int));

	blockBytes.resize(blockCount);
	blockOffset.resize(blockCount);
	inFile.read((char*)blockOffset.data(), sizeof(uint64_t) * blockCount);
	inFile.read((char*)blockBytes.data(), sizeof(uint32_t) * blockCount);

	const auto blockLength = 1 << header.blockLengthInLog;
	blockBuf = std::unique_ptr<char[]>(new char[blockLength * blockLength * blockLength]);
	compressedBuf = std::unique_ptr<char[]>(new char[blockLength * blockLength * blockLength]);


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
	inFile.seekg(blockOffset[pageID], std::ios::beg);
	inFile.read(compressedBuf.get(), blockBytes[pageID]);
	LZ4_decompress_safe(compressedBuf.get(), blockBuf.get(), blockSizeBytes, blockSizeBytes);
	return blockBuf.get();
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

std::unique_ptr<ysl::Object> ysl::LZ4FileReaderFactory::Create(const std::string& key)
{
	return std::make_unique<LZ4FileReader>();
}

std::vector<std::string> ysl::LZ4FileReaderFactory::Keys() const
{
	return { ".lz4" };
}

EXPORT_PLUGIN_FACTORY_IMPLEMENT(ysl::LZ4FileReaderFactory)

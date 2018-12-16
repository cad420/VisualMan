
#include "volume_utils.h"

BlockVolumeReader::BlockVolumeReader(const std::string& fileName): validFlag(true)
{
	fileHandle.open(fileName, std::fstream::binary);
	if (!fileHandle.is_open())
	{
		std::cout << "Can not open .lvd\n";
		validFlag = false;
		fileHandle.close();
		return;
	}
	uint32_t magicNumber;
	fileHandle.read((char*)&magicNumber, sizeof(int));
	if (magicNumber != LVDFileMagicNumber)
	{
		std::cout << " This is not a lvd file\n";
		validFlag = false;
		fileHandle.close();
		return;
	}

	fileHandle.read((char*)&vx, sizeof(int));
	fileHandle.read((char*)&vy, sizeof(int));
	fileHandle.read((char*)&vz, sizeof(int));
	fileHandle.read((char*)&logBlockSize, sizeof(int));
	fileHandle.read((char*)&m_repeat, sizeof(int));
	if (logBlockSize != LogBlockSize5)
	{
		std::cout << "Unsupported block size\n";
		validFlag = false;
		return;
	}
	const size_t ablockSize = blockSize();
	bx = ((vx + ablockSize - 1) & ~(ablockSize - 1)) / ablockSize;
	by = ((vy + ablockSize - 1) & ~(ablockSize - 1)) / ablockSize;
	bz = ((vz + ablockSize - 1) & ~(ablockSize - 1)) / ablockSize;
}

void BlockVolumeReader::readBlock(char* dest, int blockId)
{
	const size_t blockCount = blockDataCount();
	fileHandle.seekg(blockCount * blockId, std::ios::beg);
	fileHandle.read(dest, sizeof(char) * blockCount);
}

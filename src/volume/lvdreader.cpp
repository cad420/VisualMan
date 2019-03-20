
#include "lvdreader.h"

namespace ysl
{

	LVDReader::LVDReader(const std::string& fileName) : validFlag(true)
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

		int vx, vy, vz, bx, by, bz;
		int m_originalWidth, m_originalHeight, m_originalDepth;

		fileHandle.read((char*)&vx, sizeof(int));
		fileHandle.read((char*)&vy, sizeof(int));
		fileHandle.read((char*)&vz, sizeof(int));
		fileHandle.read((char*)&logBlockSize, sizeof(int));
		fileHandle.read((char*)&repeat, sizeof(int));
		fileHandle.read((char*)&m_originalWidth, sizeof(int));
		fileHandle.read((char*)&m_originalHeight, sizeof(int));
		fileHandle.read((char*)&m_originalDepth, sizeof(int));

		if (logBlockSize != LogBlockSize5 && logBlockSize != LogBlockSize6 && logBlockSize != LogBlockSize7)
		{
			std::cout << "Unsupported block size\n";
			validFlag = false;
			return;
		}

		const size_t aBlockSize = BlockSize();

		// aBlockSize must be power of 2, e.g. 32 or 64
		bx = ((vx + aBlockSize - 1) & ~(aBlockSize - 1)) / aBlockSize;
		by = ((vy + aBlockSize - 1) & ~(aBlockSize - 1)) / aBlockSize;
		bz = ((vz + aBlockSize - 1) & ~(aBlockSize - 1)) / aBlockSize;

		vSize = ysl::Size3((vx), (vy), (vz));
		bSize = ysl::Size3(bx, by, bz);
		oSize = ysl::Size3(m_originalWidth, m_originalHeight, m_originalDepth);

	}

	void LVDReader::ReadBlock(char * dest, int blockId)
	{
		const size_t blockCount = BlockDataCount();
		// 32 is the size of file header
		fileHandle.seekg(blockCount * blockId + 36, std::ios::beg);
		fileHandle.read(dest, sizeof(char) * blockCount);
	}

}
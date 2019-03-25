
#include "lvdreader.h"
#include <iostream>

namespace ysl
{

	LVDReader::LVDReader(const std::string& fileName) : validFlag(true)
	{

		std::ifstream fileHandle;

		fileHandle.open(fileName, std::fstream::binary);
		if (!fileHandle.is_open())
		{
			std::cout << "Can not open .lvd\n";
			validFlag = false;
			fileHandle.close();
			return;
		}

		unsigned char headerBuf[LVD_HEADER_SIZE];

		fileHandle.read((char*)headerBuf, LVD_HEADER_SIZE);
		header.Decode(headerBuf);

		fileHandle.close();

		//uint32_t magicNumber;
		//fileHandle.read((char*)&magicNumber, sizeof(int));

		if (header.magicNum != LVDFileMagicNumber)
		{
			std::cout << " This is not a lvd file\n";
			validFlag = false;
			fileHandle.close();
			return;
		}


		//int vx, vy, vz, bx, by, bz;
		//int m_originalWidth, m_originalHeight, m_originalDepth;

		/*fileHandle.read((char*)&vx, sizeof(int));
		fileHandle.read((char*)&vy, sizeof(int));
		fileHandle.read((char*)&vz, sizeof(int));
		fileHandle.read((char*)&logBlockSize, sizeof(int));
		fileHandle.read((char*)&repeat, sizeof(int));
		fileHandle.read((char*)&m_originalWidth, sizeof(int));
		fileHandle.read((char*)&m_originalHeight, sizeof(int));
		fileHandle.read((char*)&m_originalDepth, sizeof(int));*/

		int vx = header.dataDim[0];
		int vy = header.dataDim[1];
		int vz = header.dataDim[2];
		logBlockSize = header.blockLengthInLog;
		repeat = header.padding;

		const int originalWidth = header.originalDataDim[0];
		const int originalHeight = header.originalDataDim[1];
		const int originalDepth = header.originalDataDim[2];

		if (logBlockSize != LogBlockSize5 && logBlockSize != LogBlockSize6 && logBlockSize != LogBlockSize7)
		{
			std::cout << "Unsupported block size\n";
			validFlag = false;
			return;
		}

		const size_t aBlockSize = BlockSize();

		// aBlockSize must be power of 2, e.g. 32 or 64
		const int bx = ((vx + aBlockSize - 1) & ~(aBlockSize - 1)) / aBlockSize;
		const int by = ((vy + aBlockSize - 1) & ~(aBlockSize - 1)) / aBlockSize;
		const int bz = ((vz + aBlockSize - 1) & ~(aBlockSize - 1)) / aBlockSize;

		vSize = ysl::Size3((vx), (vy), (vz));
		bSize = ysl::Size3(bx, by, bz);
		oSize = ysl::Size3(originalWidth, originalHeight, originalDepth);

		const std::size_t bytes = std::size_t(vx) * vy*vz + LVD_HEADER_SIZE;

#ifdef _WIN32
		/// TODO:: Memeory Leak
		// I don't konw why smart pointer can not be applied here


		lvdIO = (new WindowsMappingRawIO(fileName,bytes,
			WindowsMappingRawIO::FileAccess::Read,
			WindowsMappingRawIO::MapAccess::ReadOnly ));

		lvdPtr = lvdIO->FileMemPointer(0, bytes);
		if (!lvdPtr)
			throw std::runtime_error("LVDReader: bad mapping");

#elif 
		static_assert(false);
#endif

	}

	void LVDReader::ReadBlock(char * dest, int blockId,int lod)
	{
		const size_t blockCount = BlockDataCount();
		const auto d = lvdPtr + LVD_HEADER_SIZE;

		//fileHandle.seekg(blockCount * blockId + 36, std::ios::beg);
		memcpy(dest, d + blockCount * blockId, sizeof(char) * blockCount);
		//fileHandle.read(dest, sizeof(char) * blockCount);
	}

}

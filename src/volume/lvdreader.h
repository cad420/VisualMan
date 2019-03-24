
#ifndef _LVDREADER_H_
#define _LVDREADER_H_

#include "../memory/blockarray.h"
#include "../mathematics/geometry.h"

#include <memory>
#include <fstream>
#include "lvdheader.h"
#include "rawio.h"

namespace ysl
{
	class LVDReader
	{
		//std::ifstream fileHandle;
		std::string fileName;

		LVDHeader header;
		//std::shared_ptr<AbstrRawIO> lvdIO;
		AbstrRawIO* lvdIO;
		unsigned char * lvdPtr;

		ysl::Size3 vSize;
		ysl::Size3 bSize;
		ysl::Size3 oSize;

		int logBlockSize;
		int repeat;
		bool validFlag;

		enum { LVDFileMagicNumber = 277536 };
		enum { LogBlockSize5 = 5, LogBlockSize6 = 6, LogBlockSize7 = 7 };
		enum { LVDHeaderSize = 24 };

	public:
		explicit LVDReader(const std::string& fileName);

		bool Valid()const { return validFlag; }

		ysl::Size3 Size()const { return vSize; }

		ysl::Size3 SizeByBlock()const { return bSize; }

		int BoundaryRepeat()const { return repeat; }

		int BlockSizeInLog()const { return logBlockSize; }

		int BlockSize()const { return 1 << BlockSizeInLog(); }

		int BlockDataCount()const { return BlockSize()*BlockSize()*BlockSize(); }

		int BlockCount()const { return bSize.x*bSize.y*bSize.z; }

		ysl::Size3 OriginalDataSize()const { return oSize; }

		template<typename T, int nLogBlockSize>
		std::shared_ptr<ysl::Block3DArray<T, nLogBlockSize>> ReadAll();

		void ReadBlock(char * dest, int blockId);

	};


	template <typename T, int nLogBlockSize>
	std::shared_ptr<Block3DArray<T, nLogBlockSize>> LVDReader::ReadAll()
	{
		const auto s = Size();
		const size_t bytes = s.x*s.y*s.z * sizeof(T);
		auto ptr = std::make_shared<ysl::Block3DArray<T, nLogBlockSize>>(s.x, s.y, s.z, nullptr);
		//if (ptr)
		//{
		//	fileHandle.seekg((size_t)LVDHeaderSize, std::ios::beg);
		//	fileHandle.read(ptr->Data(), bytes);
		//}
		return ptr;
	}
}

#endif
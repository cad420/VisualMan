
#ifndef _LVDREADER_H_
#define _LVDREADER_H_


#include <memory>
#include "lvdheader.h"
#include "rawio.h"
#include "blockarray.h"
#include "geometry.h"



namespace ysl
{
	class LVDReader
	{
		std::string fileName;
		LVDHeader header;
		//AbstraFileMap* lvdIO;
		unsigned char * lvdPtr;
		ysl::Size3 vSize;
		ysl::Size3 bSize;
		ysl::Size3 oSize;
		int logBlockSize;
		int padding;
		bool validFlag;
		enum { LVDFileMagicNumber = 277536 };
		enum { LogBlockSize5 = 5, LogBlockSize6 = 6, LogBlockSize7 = 7 };
		enum { LVDHeaderSize = 24 };
	public:
		explicit LVDReader(const std::string& fileName);
		LVDReader(const std::vector<std::string> & fileName,const std::vector<int> & lods = std::vector<int>{});
		bool Valid()const { return validFlag; }
		ysl::Size3 Size(int lod = 0) const { return vSize; }
		ysl::Size3 SizeByBlock(int lod = 0)const { return bSize; }
		int BoundaryRepeat(int lod = 0)const { return padding; }
		int BlockSizeInLog(int lod = 0)const { return logBlockSize; }
		int BlockSize(int lod = 0)const { return 1 << BlockSizeInLog(); }
		int BlockDataCount(int lod = 0)const { return BlockSize()*BlockSize()*BlockSize(); }
		int BlockCount(int lod = 0)const { return bSize.x*bSize.y*bSize.z; }
		ysl::Size3 OriginalDataSize(int lod = 0)const { return oSize; }
		template<typename T, int nLogBlockSize>
		std::shared_ptr<Block3DArray<T, nLogBlockSize>> ReadAll(int lod = 0);
		void ReadBlock(char * dest, int blockId,int lod = 0);
		unsigned char* ReadBlock(int blockId, int lod = 0);
		~LVDReader();
	private:
		std::unique_ptr<AbstraFileMap> lvdIO;		// the declaration must be behind the declaration of ~LVDReader()
	};

	template <typename T, int nLogBlockSize>
	std::shared_ptr<Block3DArray<T, nLogBlockSize>> LVDReader::ReadAll(int lod)
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
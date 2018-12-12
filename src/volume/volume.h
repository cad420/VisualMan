#ifndef VOLUME_H_
#define VOLUME_H_
#include <memory>
#include <string>
#include <fstream>
#include "../memory/blockarray.h"

#include <list>
#include <unordered_map>
#include <iostream>
//#include "../mathematics/arithmetic.h"


struct VolumeFormat;

// Enum Type
enum VoxelType { UInt8, Float32 };
enum VoxelFormat { Grayscale, RGB, RGBA };



//
//class VolumeDataRepresentation
//{
//public:
//	ysl::Vector3i getDimension()const;
//	VolumeFormat dataFormat()const;
//	//double sampleAt(const ysl::Point3f & pos) const;
//};


class BlockVolumeReader
{
	std::ifstream fileHandle;
	std::string fileName;
	int vx, vy, vz, bx, by, bz;
	int logBlockSize;
	bool validFlag;


	enum { LVDFileMagicNumber = 277536 };
	enum { LogBlockSize5 = 5 };
	enum { LVDHeaderSize = 64 };
public:
	BlockVolumeReader(const std::string & fileName) :validFlag(true)
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
		if(logBlockSize != LogBlockSize5)
		{
			std::cout << "Unsupported block size\n";
			validFlag = false;
			return;
		}
		const size_t ablockSize = blockSize();
		bx = ((vx + ablockSize - 1)&~(ablockSize - 1)) / ablockSize;
		by = ((vy + ablockSize - 1)&~(ablockSize - 1)) / ablockSize;
		bz = ((vz + ablockSize - 1)&~(ablockSize - 1)) / ablockSize;

	}

	bool valid()const { return validFlag; }

	int width()const { return vx; }
	int height()const { return vy; }
	int depth()const { return vz; }

	int xBlockCount()const { return bx; }
	int yBlockCount()const { return by; }
	int zBlockCount()const { return bz; }

	int blockSizeInLog()const { return logBlockSize; }
	int blockSize()const { return 1 << blockSizeInLog(); }
	int blockDataCount()const { return blockSize()*blockSize()*blockSize(); }

	int totalBlocks()const { return bx * by * bz; }

	void readBlock(char * dest, int blockId)
	{
		const size_t blockCount = blockDataCount();
		fileHandle.seekg(blockCount*blockId, std::fstream::_Seekbeg);
		fileHandle.read(dest, sizeof(char)*blockCount);
	}

};



class LargeVolumeCache
{
	// VolumeDataReader
	struct LRUListCell;
	using LRUHash = std::unordered_map<int,std::list<LRUListCell>::iterator>;
	struct LRUListCell
	{
		int blockCacheIndex;
		LRUHash::iterator hashIter;
		LRUListCell(int index):blockCacheIndex(index){}
	};

	BlockVolumeReader m_blockVolumeReader;
	const uint32_t m_logBlockSize;
	const int m_vx, m_vy, m_vz;
	std::list<LRUListCell> m_lruList;
	LRUHash	m_blockIdInCache;		// blockId---> (blockIndex,the position of blockIndex in queue)

	ysl::Block3DArray<unsigned char, 5> m_volumeCached;

	int blockCoordinateToBlockId(int xBlock, int yBlock, int zBlock)const
	{
		const auto nxBlock = m_vx / m_volumeCached.BlockSize(), nyBlock = m_vy / m_volumeCached.BlockSize(), nzBlock = m_vz / m_volumeCached.BlockSize();
		return zBlock * nxBlock*nyBlock + yBlock * nxBlock + zBlock;
	}

	static constexpr size_t cacheWidth = 64;
	static constexpr size_t cacheHeight = 64;
	static constexpr size_t cacheDepth = 64;
	static constexpr size_t totalCacheBlocks = 2*2*2;

public:
	explicit LargeVolumeCache(const std::string & fileName) :
		m_blockVolumeReader(fileName),
		m_logBlockSize(m_blockVolumeReader.blockSizeInLog()),
		m_vx(m_blockVolumeReader.width()),
		m_vy(m_blockVolumeReader.height()),
		m_vz(m_blockVolumeReader.depth()),
		m_volumeCached(cacheWidth, cacheHeight, cacheDepth,nullptr)
	{
		for (int i = 0; i < totalCacheBlocks; i++)
		{
			m_lruList.push_front(LRUListCell(i));
		}
	}

	const BlockVolumeReader & reader()const { return m_blockVolumeReader; }
	const unsigned char * blockData(int xBlock, int yBlock, int zBlock) { return blockData(blockCoordinateToBlockId(xBlock, yBlock, zBlock)); }
	const unsigned char * blockData(int blockId);

};



//class LargeVolumeRepresentation
//{
//	
//};
//
//
//class UnstructuredVolumeRepresentation
//{
//	
//};
//
//class BlockVolumeRepresentation
//{
//	
//};



/**
 * \brief The volume data reader give the basic information about a volume data file,
 * and it provide some method for reading data of different types
 */

 //class VolumeDataReader
 //{
 //	std::ifstream m_fileHandle;
 //	std::string m_fileName;
 //	//std::unique_ptr<char[]> m_buffer;
 //	//std::vector<void> m_pool;
 //public:
 //	VolumeDataReader(const std::string & fileName, const VolumeFormat & fmt);
 //	VolumeDataReader(const VolumeDataReader & reader) = delete;
 //	VolumeDataReader & operator=(const VolumeDataReader & reader) = delete;
 //	VolumeDataReader(VolumeDataReader && reader)noexcept
 //	{
 //		m_fileHandle = std::move(reader.m_fileHandle);
 //		//m_buffer = std::move(reader.m_buffer);
 //	}
 //	VolumeDataReader & operator=(VolumeDataReader && reader)noexcept
 //	{
 //		m_fileHandle = std::move(reader.m_fileHandle);
 //		//m_buffer = std::move(reader.m_buffer);
 //		return *this;
 //	}
 //
 //	std::string fileName()const
 //	{
 //		return m_fileName;
 //	}
 //
 //
 //
 //	//void * wholeData();
 //	//ysl::Vector3i getDimension()const;
 //	//VolumeFormat dataFormat()const;
 //	//void * subData(const ysl::Point3i & min, const ysl::Point3i & max);
 //	//double sampleAt(const ysl::Point3f & pos) const;
 //
 //	//template<typename T>
 //	//std::shared_ptr<VolumeDataRepresentation> volumeData()const
 //	//{
 //	//	
 //	//}
 //
 //	//void readData(VolumeDataRepresentation & repr);
 //
 //
 //
 //	~VolumeDataReader() = default;
 //};
 //
 //






#endif
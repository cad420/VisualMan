#ifndef _VOLUME_UTILS_H_
#define _VOLUME_UTILS_H_

#include "../memory/blockarray.h"
#include "../mathematics/geometry.h"
#include "../core/spectrum.h"
#include <memory>
#include <fstream>
#include <iostream>
#include <functional>
#include "appearance.h"

class LVDFileTraits
{
public:
	using MagicNumber_t = int;
	using DimensionSize_t = int;
	using LogBlockSize_t = int;
	using BoundarySize_t = int;
	static constexpr int HeaderSize = 24;
	static constexpr int MagicNumber = 277536;
};

struct LVDFileHeader
{
	uint32_t magicNumber;	//  4
	uint32_t vSize[3];		//	12
	uint32_t oSize[3];		//  12
	uint32_t repeat;		//  4
	uint32_t logBlockSize;	//  4
	char reserved[28];		// 28	 == 64bytes in total
};

//#define WRITE_SINGLE_BLOCK

template<int nLogBlockSize, typename LVDTraits = LVDFileTraits>
class RawToLVDConverter
{
	using RawType = char;

	//ysl::Size3 size;
	//ysl::Size3 offset;
	//ysl::Size3 blockSize;

	int g_xOffset, g_yOffset, g_zOffset;
	int g_xSize, g_ySize, g_zSize;
	int g_xBlockSize, g_yBlockSize, g_zBlockSize;
	RawType g_emptyValue;

	//std::unique_ptr<ysl::Block3DArray<RawType, nLogBlockSize>> m_blockedData;

	void getData(RawType * dest, const RawType * src, size_t width, size_t height, size_t depth, size_t xb, size_t yb, size_t zb)const
	{


		//#pragma omp parallel for

		// write temp file
#ifdef WRITE_SINGLE_BLOCK
		std::string fileName{ "D:\\scidata\\abc\\temp\\" };

		const std::size_t bytes = std::size_t(depth) * height*width;
		std::unique_ptr<RawType[]> buf(new RawType[bytes]);
#endif


		for (int z = 0; z < depth; z++)
			for (int y = 0; y < height; y++)
				for (int x = 0; x < width; x++)
				{
					const int gx = x + g_xOffset, gy = y + g_yOffset, gz = z + g_zOffset;
					const size_t blockIndex = x + y * g_xBlockSize + z * g_xBlockSize*g_yBlockSize;


					int blockedGlobalX = x + xb * g_xBlockSize;
					int blockedGlobalY = y + yb * g_yBlockSize;
					int blockedGlobalZ = z + zb * g_zBlockSize;


					if (gx >= 0 && gx < g_xSize && gy >= 0 && gy < g_ySize && gz >= 0 && gz < g_zSize)
					{
						const size_t linearIndex = (gx)+(gy)*std::size_t(g_xSize) + (gz)*g_xSize*std::size_t(g_ySize);
						*(dest + blockIndex) = *(src + linearIndex);
						///TODO::
#ifdef WRITE_SINGLE_BLOCK
						buf[blockIndex] = *(src + linearIndex);
#endif

						//(*m_blockedData)(blockedGlobalX, blockedGlobalY, blockedGlobalZ) = *(src + linearIndex);
					}
					else
					{
#ifdef WRITE_SINGLE_BLOCK
						buf[blockIndex] = g_emptyValue;
#endif
						//(*m_blockedData)(blockedGlobalX, blockedGlobalY, blockedGlobalZ) = g_emptyValue;
						///TODO::
						*(dest + blockIndex) = g_emptyValue;
					}
				}
#ifdef  WRITE_SINGLE_BLOCK
		std::stringstream ss;
		ss << xb << "." << yb << "." << zb << ".raw";
		std::string str;
		ss >> str;
		std::ofstream outFile(fileName + str);
		outFile.write((char*)buf.get(), bytes);
		std::cout << "Write single block finished\n";
#endif


	}

	std::ifstream m_rawFile;
	std::ofstream m_lvdFile;
	ysl::Vector3i m_dataSize;
	ysl::Vector3i m_blockDimension;
	std::unique_ptr<RawType[]> m_rawBuf;
	std::unique_ptr<RawType[]> m_lvdBuf;
	const int m_repeat;
	static constexpr auto blockSize = 1 << nLogBlockSize;
public:
	RawToLVDConverter(const std::string & fileName, int xSize, int ySize, int zSize, int repeat) :
		g_xSize(xSize),
		g_ySize(ySize),
		g_zSize(zSize),
		g_emptyValue(0),
		m_repeat(repeat),
		g_xBlockSize(blockSize),
		g_yBlockSize(blockSize),
		g_zBlockSize(blockSize)
	{
		if (repeat < 0 || repeat > 2)
		{
			std::cout << "Unsupported repeat\n";
			return;
		}

		std::ifstream rawFile(fileName, std::ifstream::binary);

		if (!rawFile.is_open())
		{
			std::cout << "can not open raw file.\n";
			return;
		}


		const auto blockedSize = blockSize - 2 * m_repeat;
		m_blockDimension = { int(ysl::RoundUpDivide(xSize, blockedSize)), int(ysl::RoundUpDivide(ySize, blockedSize)) , int(ysl::RoundUpDivide(zSize, blockedSize)) };
		std::cout << "Raw data should be divided in:" << m_blockDimension << std::endl;

		//m_newDataDimension = { xBlockedCount * blockSize ,yBlockedCount * blockSize ,zBlockedCount * blockSize };
		m_dataSize = m_blockDimension * ysl::Vector3i{ blockSize,blockSize,blockSize };

		std::cout << "lvd data dimension: " << m_dataSize << std::endl;

		const auto rawBytes = std::size_t(xSize) * ySize * zSize * sizeof(RawType);
		if (rawBytes == 0)
		{
			std::cout << "empty data.\n";
			return;
		}
		m_rawBuf.reset(new RawType[rawBytes]);
		if (!m_rawBuf)
		{
			std::cout << " bad alloc." << __LINE__ << std::endl;
			return;
		}

		const auto lvdBytes = size_t(m_dataSize.x)*size_t(m_dataSize.y)*size_t(m_dataSize.z) * sizeof(RawType);

		///TODO::
		//auto p = new ysl::Block3DArray<RawType, nLogBlockSize>(m_dataSize.x, m_dataSize.y, m_dataSize.z, nullptr);
		//if (p == nullptr) {
		//	std::cout << " bad alloc." << __LINE__ << std::endl;
		//	return;
		//}
		//m_blockedData.reset(p);

		m_lvdBuf.reset(new RawType[lvdBytes]);
		if (!m_lvdBuf)
		{
			std::cout << " bad alloc." << __LINE__ << std::endl;
			return;
		}


		std::cout << "Reading raw file...\n";
		rawFile.read(m_rawBuf.get(), rawBytes);
		std::cout << "Reading file completes.\n";

	}
	auto dimensionInData()const { return m_dataSize; }
	auto dimensionInBlock()const { return m_blockDimension; }
	void setBoundaryValue(RawType value) { g_emptyValue = value; }
	bool convert()
	{
		g_emptyValue = 0;
		const auto step = blockSize - 2 * m_repeat;
		for (int zb = 0; zb < m_blockDimension.z; zb++)
			for (int yb = 0; yb < m_blockDimension.y; yb++)
				for (int xb = 0; xb < m_blockDimension.x; xb++)
				{
					g_xOffset = -m_repeat + xb * step;
					g_yOffset = -m_repeat + yb * step;
					g_zOffset = -m_repeat + zb * step;
					const int blockIndex = xb + yb * m_blockDimension.x + zb * m_blockDimension.x*m_blockDimension.y;
					getData(m_lvdBuf.get() + blockIndex * size_t(blockSize)*blockSize*blockSize, m_rawBuf.get(), blockSize, blockSize, blockSize, xb, yb, zb);
					if(blockIndex % 100 ==0)
					{
						ysl::Log("%lf Completion\n", blockIndex % 100);
					}
				}
		return true;
	}

	bool save(const std::string & fileName)
	{
		std::stringstream ss1;
		ss1 << m_repeat;
		std::string rep,bSize;
		ss1 >> rep;
		std::stringstream ss2;
		ss2 << (1<<nLogBlockSize);
		ss2 >> bSize;

		const std::string outFileName{ fileName.substr(0,fileName.find_last_of("."))+"_"+rep+"_"+bSize + ".lvd" };
		std::cout << "Writing as:" << outFileName << std::endl;
		std::ofstream lvdFile(outFileName, std::fstream::binary);
		if (!lvdFile.is_open())
		{
			std::cout << ".lvd file can not open\n";
			return false;
		}

		const auto lvdBytes = size_t(m_dataSize.x)*size_t(m_dataSize.y)*size_t(m_dataSize.z) * sizeof(RawType);

		constexpr auto logBlockSize = nLogBlockSize;
		lvdFile.write((char*)&LVDTraits::MagicNumber, 4);
		lvdFile.write((char*)&m_dataSize.x, 3 * sizeof(LVDTraits::DimensionSize_t));
		lvdFile.write((char*)&logBlockSize, sizeof(LVDTraits::LogBlockSize_t));
		lvdFile.write((char*)&m_repeat, sizeof(LVDTraits::BoundarySize_t));
		lvdFile.write((char*)&g_xSize, sizeof(LVDTraits::DimensionSize_t));
		lvdFile.write((char*)&g_ySize, sizeof(LVDTraits::DimensionSize_t));
		lvdFile.write((char*)&g_zSize, sizeof(LVDTraits::DimensionSize_t));

		///TODO::
		lvdFile.write(m_lvdBuf.get(), lvdBytes);
		std::cout << "writing .lvd file finished\n";
		return true;
	}
};

//struct LVDFileHeader
//{
//	uint32_t magicNumber;	//  4
//	uint32_t vSize[3];		//	12
//	uint32_t oSize[3];		//  12
//	uint32_t repeat;		//  4
//	uint32_t logBlockSize;	//  4
//	char reserved[28];		// 28	 == 64bytes in total
//};

class RawReader
{
	std::string fileName;
	ysl::Size3 dimensions;
	size_t voxelSize;
	FILE *file;
	int64_t offset;

public:
	RawReader(const std::string &fileName,
		const ysl::Size3 &dimensions, size_t voxelSize);
	~RawReader();
	// Read a region of volume data from the file into the buffer passed.
	// It's assumed the buffer passed has enough room. Returns the
	// number voxels read
	size_t readRegion(const ysl::Size3 & start,
		const ysl::Size3 &size, unsigned char *buffer);

private:
	inline bool convexRead(const ysl::Size3 & size) {
		// 3 cases for convex reads:
		// - We're reading a set of slices of the volume
		// - We're reading a set of scanlines of a slice
		// - We're reading a set of voxels in a scanline
		return (size.x == dimensions.x && size.y == dimensions.y)
			|| (size.x == dimensions.x && size.z == 1)
			|| (size.y == 1 && size.z == 1);
	}
};

/**
 * \brief This should be designed as a boundary-repeated block reader
 */



class LVDReader
{
	std::ifstream fileHandle;
	std::string fileName;

	ysl::Size3 vSize;
	ysl::Size3 bSize;
	ysl::Size3 oSize;
	//int vx, vy, vz, bx, by, bz;
	//int m_originalWidth, m_originalHeight, m_originalDepth;
	int logBlockSize;
	int repeat;
	bool validFlag;

	enum { LVDFileMagicNumber = 277536 };

	enum { LogBlockSize5 = 5, LogBlockSize6 = 6,LogBlockSize7 = 7 };

	enum { LVDHeaderSize = 24 };

public:
	explicit LVDReader(const std::string& fileName);

	bool valid()const { return validFlag; }

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
std::shared_ptr<ysl::Block3DArray<T, nLogBlockSize>> LVDReader::ReadAll()
{
	const auto s = Size();
	const size_t bytes = s.x*s.y*s.z * sizeof(T);
	auto ptr = std::make_shared<ysl::Block3DArray<T, nLogBlockSize>>(s.x, s.y, s.z, nullptr);
	if (ptr)
	{
		fileHandle.seekg((size_t)LVDHeaderSize, std::ios::beg);
		fileHandle.read(ptr->Data(), bytes);
	}
	return ptr;
}

namespace ysl
{
	class MappingKey
	{
	public:
		Float intensity;
		RGBASpectrum leftColor;
		RGBASpectrum rightColor;
		MappingKey(Float intensity, const RGBASpectrum & lc, const RGBASpectrum & rc) :intensity(intensity), leftColor(lc), rightColor(rc) {}
		Float Intensity()const { return intensity; }
		RGBASpectrum LeftColor()const { return leftColor; }
		RGBASpectrum RightColor()const { return rightColor; }
		void SetIntensity(Float intensity) { intensity = intensity; }
		void SetLeftColor(const RGBASpectrum & c)
		{
			const auto a = leftColor.c[3];
			leftColor = c;
			leftColor.c[3] = a;

		}
		void SetRightColor(const RGBASpectrum & c)
		{
			const auto a = leftColor.c[3];
			rightColor = c;
			leftColor.c[3] = a;
		}
		void SetLeftAlpha(Float a) { leftColor.c[3] = a; }
		void SetRightAlpha(Float a) { rightColor.c[3] = a; }
	};

	class ColorInterpulator
	{
	public:
		ColorInterpulator() :m_valid(false) {}
		explicit ColorInterpulator(const std::string & fileName) :m_valid(false)
		{
			read(fileName);
		}

		void read(const std::string& fileName);
		bool valid()const
		{
			return m_valid;
		}
		void FetchData(RGBASpectrum* transferFunction, int dimension)const;
		void FetchData(Float * transferFunction, int dimension)const
		{
			FetchData(reinterpret_cast<RGBASpectrum*>(transferFunction), dimension);
		}
		int KeysCount()const
		{
			return keys.size();
		}
		//const MappingKey & Key()const
		//{
		//	
		//}

		const MappingKey & operator[](int i)const
		{
			return keys[i];
		}

		MappingKey & operator[](int i)
		{
			return keys[i];
		}

	private:
		bool m_valid;
	protected:
		std::vector<MappingKey> keys;
		Float leftThreshold;
		Float rightThreshold;
	};


	class TransferFunction:public ColorInterpulator
	{
	public:
		using Callback = std::function<void(TransferFunction* tf)>;
		TransferFunction() = default;
		TransferFunction(const std::string & fileName);
		~TransferFunction();
		void AddMappingKey(const MappingKey & key);
		void UpdateMappingKey(const Point2f & at, const Point2f & to,const Vector2f & scale = {1.0f,1.0f});
		void UpdateMappingKey(const Point2f & at, const MappingKey & key,const Vector2f & scale = {1.0f,1.0f});
		void AddUpdateCallBack(const Callback & func);
		Point2f KeyPosition(int i, Float sx, Float sy)const;
		int HitAt(const Point2f & at, const Vector2f & scale);
		void UpdateMappingKey(int index, const RGBASpectrum& color);
	private:
		std::vector<Callback> callbacks;
		std::function<bool(const MappingKey & m1, const MappingKey & m2)> compareFunc;
		void Notify();
		static Point2f TransToPos(const MappingKey & key);
	};

}


#endif

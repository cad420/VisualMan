
#ifndef _LVDCONVERTER_H_
#define _LVDCONVERTER_H_

#include "../mathematics/geometry.h"
#include "../mathematics/numeric.h"

#include "rawio.h"
#include "lvdheader.h"


namespace ysl
{

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

	template<int nLogBlockSize, typename LVDTraits = LVDFileTraits>
	class RawToLVDConverter
	{
		using RawType = unsigned char;

		//ysl::Size3 size;
		//ysl::Size3 offset;
		//ysl::Size3 blockSize;

		int g_xOffset, g_yOffset, g_zOffset;
		int g_xSize, g_ySize, g_zSize;
		int g_xBlockSize, g_yBlockSize, g_zBlockSize;
		RawType g_emptyValue;

		void getData(RawType* dest, const RawType* src, size_t width, size_t height, size_t depth, size_t xb, size_t yb,
		             size_t zb) const;

		std::ifstream m_rawFile;
		std::ofstream m_lvdFile;


		ysl::Vector3i m_dataSize;
		ysl::Vector3i m_blockDimension;

		//std::unique_ptr<RawType[]> m_rawBuf;
		std::shared_ptr<ysl::AbstrRawIO> rawIO;
		unsigned char * rawPtr;

		//std::unique_ptr<RawType[]> m_lvdBuf;
		std::shared_ptr<ysl::AbstrRawIO> lvdIO;
		unsigned char * lvdPtr;

		const int m_repeat;
		static constexpr auto blockSize = 1 << nLogBlockSize;

	public:
		RawToLVDConverter(const std::string& fileName, int xSize, int ySize, int zSize, int repeat,const std::string & outFileName);
		auto DimensionInData()const { return m_dataSize; }
		auto DimensionInBlock()const { return m_blockDimension; }
		void setBoundaryValue(RawType value) { g_emptyValue = value; }

		bool convert();
		bool save(const std::string& fileName);
	};

	template <int nLogBlockSize, typename LVDTraits>
	void RawToLVDConverter<nLogBlockSize, LVDTraits>::getData(RawType* dest, const RawType* src, size_t width,
	                                                          size_t height, size_t depth, size_t xb, size_t yb,
	                                                          size_t zb) const
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
					const size_t blockIndex = x + y * g_xBlockSize + z * g_xBlockSize * g_yBlockSize;

					int blockedGlobalX = x + xb * g_xBlockSize;
					int blockedGlobalY = y + yb * g_yBlockSize;
					int blockedGlobalZ = z + zb * g_zBlockSize;

					if (gx >= 0 && gx < g_xSize && gy >= 0 && gy < g_ySize && gz >= 0 && gz < g_zSize)
					{
						const size_t linearIndex = (gx) + (gy) * std::size_t(g_xSize) + (gz) * g_xSize * std::size_t(
							g_ySize);
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
						//assert((gx>=-2 && gx<0) || (gx >= 60 && gx<62) || (gy >= -2 && gy < 0) || (gy >= 60 && gy < 62) || (gz >= -2 && gz < 0) || (gz >= 60 && gz < 62));
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

	template <int nLogBlockSize, typename LVDTraits>
	RawToLVDConverter<nLogBlockSize, LVDTraits>::RawToLVDConverter(const std::string& fileName, int xSize, int ySize,
	                                                               int zSize, int repeat,const std::string & outFileName):
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

		const auto blockedSize = blockSize - 2 * m_repeat;
		m_blockDimension = { int(ysl::RoundUpDivide(xSize, blockedSize)), int(ysl::RoundUpDivide(ySize, blockedSize)) , int(ysl::RoundUpDivide(zSize, blockedSize)) };
		//m_blockDimension = {int(xSize / blockedSize), int(ySize / blockedSize), int(zSize / blockedSize)};

		std::cout << "Information:" << std::endl;
		std::cout << g_xSize << " " << g_ySize << " " << g_zSize << std::endl;
		std::cout << "block size:" << blockedSize << std::endl;
		m_dataSize = m_blockDimension * ysl::Vector3i{blockSize, blockSize, blockSize};

		std::cout << "lvd block dimension:" << m_blockDimension << std::endl;
		std::cout << "lvd data dimension: " << m_dataSize << std::endl;
		const auto rawBytes = std::size_t(xSize) * ySize * zSize * sizeof(RawType);
		if (rawBytes == 0)
		{
			std::cout << "empty data.\n";
			return;
		}

#ifdef _WIN32
		rawIO = std::make_shared<WindowsMappingRawIO>(fileName,
			rawBytes,
			WindowsMappingRawIO::Read, 
			WindowsMappingRawIO::ReadOnly);

		rawPtr = rawIO->FileMemPointer(0, rawBytes);
		if(!rawPtr)
		{
			throw std::runtime_error("raw file bad mapping");
		}
		const auto lvdBytes = size_t(m_dataSize.x) * size_t(m_dataSize.y) * size_t(m_dataSize.z) * sizeof(RawType);

		lvdIO = std::make_shared<WindowsMappingRawIO>(outFileName,
			lvdBytes+LVD_HEADER_SIZE,WindowsMappingRawIO::FileAccess::Write | WindowsMappingRawIO::FileAccess::Read,
			WindowsMappingRawIO::ReadWrite);
		lvdPtr = lvdIO->FileMemPointer(0, lvdBytes + LVD_HEADER_SIZE);

		if (!lvdPtr)
		{
			throw std::runtime_error("lvd file bad mapping");
		}
#elif
		static_assert(false);
#endif
	}

	template <int nLogBlockSize, typename LVDTraits>
	bool RawToLVDConverter<nLogBlockSize, LVDTraits>::convert()
	{
		// Allocate a buffer for block data with padding 

		if(!lvdPtr)
		{
			std::cout << "Bad lvd data pointer\n";
			return false;
		}

		auto const buf = lvdPtr + LVD_HEADER_SIZE;

		g_emptyValue = 0;
		const auto step = blockSize - 2 * m_repeat;
		for (int zb = 0; zb < m_blockDimension.z; zb++)
			for (int yb = 0; yb < m_blockDimension.y; yb++)
				for (int xb = 0; xb < m_blockDimension.x; xb++)
				{
					g_xOffset = -m_repeat + xb * step;
					g_yOffset = -m_repeat + yb * step;
					g_zOffset = -m_repeat + zb * step;

					const int blockIndex = xb + yb * m_blockDimension.x + zb * m_blockDimension.x * m_blockDimension.y;

					getData(buf + blockIndex * size_t(blockSize) * blockSize * blockSize,
						rawPtr,
					    blockSize,
						blockSize,
						blockSize,
						xb, 
						yb, 
						zb);


					if (blockIndex % 100 == 0)
					{
						std::cout << blockIndex << " " << "block completion" << std::endl;
					}
				}
		return true;
	}

	template <int nLogBlockSize, typename LVDTraits>
	bool RawToLVDConverter<nLogBlockSize, LVDTraits>::save(const std::string& fileName)
	{

		const auto rep = std::to_string(m_repeat), bSize = std::to_string(1<<nLogBlockSize);
		const auto lvdBytes = size_t(m_dataSize.x) * size_t(m_dataSize.y) * size_t(m_dataSize.z) * sizeof(RawType);
		constexpr auto logBlockSize = nLogBlockSize;

		// 36 bytes in total

		LVDHeader lvdHeader;
		lvdHeader.magicNum = LVDTraits::MagicNumber;
		lvdHeader.dataDim[0] = m_dataSize.x;
		lvdHeader.dataDim[1] = m_dataSize.y;
		lvdHeader.dataDim[2] = m_dataSize.z;
		lvdHeader.padding = m_repeat;
		lvdHeader.blockLengthInLog = logBlockSize;

		lvdHeader.originalDataDim[0] = m_blockDimension.x * ( (1 << nLogBlockSize) - 2 * m_repeat);
		lvdHeader.originalDataDim[1] = m_blockDimension.y * ( (1 << nLogBlockSize) - 2 * m_repeat);
		lvdHeader.originalDataDim[2] = m_blockDimension.z * ((1 << nLogBlockSize) - 2 * m_repeat);

		const auto p = lvdHeader.Encode();

		std::cout << m_dataSize << " " <<
			logBlockSize << " " <<
			m_repeat << " " << 
			g_xSize << " " << 
			g_ySize << " " <<
			g_zSize << std::endl;

		// Write lvd header
		memcpy(lvdPtr, p, LVD_HEADER_SIZE);

		std::cout << "writing .lvd file finished\n";
		return true;
	}
}

#endif/*_LVDCONVERTER_H_*/
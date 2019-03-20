
#ifndef _LVDCONVERTER_H_
#define _LVDCONVERTER_H_

#include "../mathematics/geometry.h"


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
		using RawType = char;

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

		std::unique_ptr<RawType[]> m_rawBuf;
		std::unique_ptr<RawType[]> m_lvdBuf;

		const int m_repeat;

		static constexpr auto blockSize = 1 << nLogBlockSize;

	public:
		RawToLVDConverter(const std::string& fileName, int xSize, int ySize, int zSize, int repeat);
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
	                                                               int zSize, int repeat):
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
		//m_blockDimension = { int(ysl::RoundUpDivide(xSize, blockedSize)), int(ysl::RoundUpDivide(ySize, blockedSize)) , int(ysl::RoundUpDivide(zSize, blockedSize)) };
		m_blockDimension = {int(xSize / blockedSize), int(ySize / blockedSize), int(zSize / blockedSize)};
		std::cout << "Raw data should be divided in:" << m_blockDimension << std::endl;

		//m_newDataDimension = { xBlockedCount * blockSize ,yBlockedCount * blockSize ,zBlockedCount * blockSize };
		m_dataSize = m_blockDimension * ysl::Vector3i{blockSize, blockSize, blockSize};

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

		const auto lvdBytes = size_t(m_dataSize.x) * size_t(m_dataSize.y) * size_t(m_dataSize.z) * sizeof(RawType);

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

	template <int nLogBlockSize, typename LVDTraits>
	bool RawToLVDConverter<nLogBlockSize, LVDTraits>::convert()
	{

		// Allocate a buffer for block data with padding 


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

					getData(m_lvdBuf.get() + blockIndex * size_t(blockSize) * blockSize * blockSize,
						m_rawBuf.get(),
					    blockSize,
						blockSize,
						blockSize,
						xb, 
						yb, 
						zb);


					if (blockIndex % 100 == 0)
					{
						ysl::Log("%d block Completion\n", blockIndex);
					}
				}
		return true;
	}

	template <int nLogBlockSize, typename LVDTraits>
	bool RawToLVDConverter<nLogBlockSize, LVDTraits>::save(const std::string& fileName)
	{
		std::stringstream ss1;
		ss1 << m_repeat;
		std::string rep, bSize;
		ss1 >> rep;
		std::stringstream ss2;
		ss2 << (1 << nLogBlockSize);
		ss2 >> bSize;

		const std::string outFileName{
			fileName.substr(0, fileName.find_last_of(".")) + "_" + rep + "_" + bSize + ".lvd"
		};
		std::cout << "Writing as:" << outFileName << std::endl;
		std::ofstream lvdFile(outFileName, std::fstream::binary);
		if (!lvdFile.is_open())
		{
			std::cout << ".lvd file can not open\n";
			return false;
		}

		const auto lvdBytes = size_t(m_dataSize.x) * size_t(m_dataSize.y) * size_t(m_dataSize.z) * sizeof(RawType);

		constexpr auto logBlockSize = nLogBlockSize;

		lvdFile.write((char*)&LVDTraits::MagicNumber, 4);
		lvdFile.write((char*)&m_dataSize.x, 3 * sizeof(LVDTraits::DimensionSize_t));
		lvdFile.write((char*)&logBlockSize, sizeof(LVDTraits::LogBlockSize_t));
		lvdFile.write((char*)&m_repeat, sizeof(LVDTraits::BoundarySize_t));
		lvdFile.write((char*)&g_xSize, sizeof(LVDTraits::DimensionSize_t));
		lvdFile.write((char*)&g_ySize, sizeof(LVDTraits::DimensionSize_t));
		lvdFile.write((char*)&g_zSize, sizeof(LVDTraits::DimensionSize_t));

		std::cout << m_dataSize << " " << logBlockSize << " " << m_repeat << " " << g_xSize << " " << g_ySize << " " <<
			g_zSize << std::endl;


		///TODO::
		lvdFile.write(m_lvdBuf.get(), lvdBytes);
		std::cout << "writing .lvd file finished\n";
		return true;
	}
}

#endif/*_LVDCONVERTER_H_*/
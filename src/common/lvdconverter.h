
#ifndef _LVDCONVERTER_H_
#define _LVDCONVERTER_H_

#include <cstring>
#include <fstream>

#include "geometry.h"
#include "numeric.h"
#include "lvdheader.h"
#include "rawio.h"
#include "libraryloader.h"
#include "timer.h"
#include "error.h"
#include <atomic>
#include "threadpool.h"
#include "rawreader.h"
#include <omp.h>

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
		//int g_xOffset, g_yOffset, g_zOffset;
		int g_xSize, g_ySize, g_zSize;
		int m_step;
		int g_xBlockSize, g_yBlockSize, g_zBlockSize;
		RawType g_emptyValue = 0;
		std::size_t rawPointerOffset;




		void getData(RawType* dest,
			const RawType* src,
			size_t width,
			size_t height,
			size_t depth,
			size_t xb,
			size_t yb,
			size_t zb) const;

		static void GetData(RawType * dest,
			const RawType * src, int width, int height, int depth,
			int w,
			int h,
			int d,
			int xb,
			int yb,
			int zb,
			int xbegin,
			int ybegin,
			int cbegin,
			int xstep,
			int ystep,
			int zstep,
			RawType fill);


		std::ifstream m_rawFile;
		std::ofstream m_lvdFile;


		ysl::Vector3i m_dataSize;
		ysl::Vector3i m_blockDimension;

		//std::unique_ptr<RawType[]> m_rawBuf;
		std::shared_ptr<ysl::IPluginFileMap> rawIO;
		unsigned char * rawPtr;

		//std::unique_ptr<RawType[]> m_lvdBuf;
		std::shared_ptr<ysl::IPluginFileMap> lvdIO;
		unsigned char * lvdPtr;

		const int m_padding;
		static constexpr auto blockSize = 1 << nLogBlockSize;

	public:
		RawToLVDConverter(const std::string& fileName, int xSize, int ySize, int zSize, int repeat, const std::string & outFileName, std::size_t offset = 0);
		auto DimensionInData()const { return m_dataSize; }
		auto DimensionInBlock()const { return m_blockDimension; }
		void setBoundaryValue(RawType value) { g_emptyValue = value; }
		bool convert();
		bool convertMultithread();
		bool save(const std::string& fileName);

	};

	template <int nLogBlockSize, typename LVDTraits>
	void RawToLVDConverter<nLogBlockSize, LVDTraits>::getData(RawType* dest,
		const RawType* src,
		size_t width,
		size_t height,
		size_t depth,
		size_t xb,
		size_t yb,
		size_t zb) const
	{
		//#pragma omp parallel for

		for (int z = 0; z < depth; z++)
			for (int y = 0; y < height; y++)
				for (int x = 0; x < width; x++)
				{
					const int xOffset = -m_padding + xb * m_step;
					const int yOffset = -m_padding + yb * m_step;
					const int zOffset = -m_padding + zb * m_step;
					const int gx = x + xOffset, gy = y + yOffset, gz = z + zOffset;
					const size_t blockIndex = x + y * g_xBlockSize + z * g_xBlockSize * g_yBlockSize;
					if (gx >= 0 && gx < g_xSize && gy >= 0 && gy < g_ySize && gz >= 0 && gz < g_zSize)
					{
						const size_t linearIndex = (gx)+(gy)* std::size_t(g_xSize) + (gz)* g_xSize * std::size_t(
							g_ySize);
						*(dest + blockIndex) = *(src + linearIndex);
					}
					else
					{
						*(dest + blockIndex) = g_emptyValue;
					}
				}
	}

	template <int nLogBlockSize, typename LVDTraits>
	void RawToLVDConverter<nLogBlockSize, LVDTraits>::GetData(RawType* dest,
		const RawType* src,
		int width,
		int height,
		int depth,
		int sw,
		int sh,
		int sd,

		int xb,
		int yb,
		int zb,

		int xbegin,
		int ybegin,
		int zbegin,

		int xstep,
		int ystep,
		int zstep,
		RawType fill)
	{
		for (int z = 0; z < sd; z++)
			for (int y = 0; y < sh; y++)
				for (int x = 0; x < sw; x++)
				{
					const int xOffset = xbegin + xb * xstep;
					const int yOffset = ybegin + yb * ystep;
					const int zOffset = zbegin + zb * zstep;
					const int gx = x + xOffset, gy = y + yOffset, gz = z + zOffset;
					const size_t blockIndex = x + y * (size_t)sw + z * (size_t)sw * (size_t)sh;
					if (gx >= 0 && gx < width && gy >= 0 && gy < height && gz >= 0 && gz < depth)
					{
						const size_t linearIndex = (gx)+(gy)* std::size_t(width) + (gz)* width * std::size_t(height);
						*(dest + blockIndex) = *(src + linearIndex);
					}
					else
					{
						*(dest + blockIndex) = fill;
					}
				}
	}


	template <int nLogBlockSize, typename LVDTraits>
	RawToLVDConverter<nLogBlockSize, LVDTraits>::RawToLVDConverter(const std::string& fileName, int xSize, int ySize,
		int zSize, int padding, const std::string & outFileName, std::size_t offset) :
		g_xSize(xSize),
		g_ySize(ySize),
		g_zSize(zSize),
		g_emptyValue(0),
		m_padding(padding),
		g_xBlockSize(blockSize),
		g_yBlockSize(blockSize),
		g_zBlockSize(blockSize),
		rawPointerOffset(offset)
	{
		if (padding < 0 || padding > 2)
		{
			std::cout << "Unsupported repeat\n";
			return;
		}

		const auto blockedSize = blockSize - 2 * m_padding;
		m_blockDimension = { int(ysl::RoundUpDivide(xSize, blockedSize)), int(ysl::RoundUpDivide(ySize, blockedSize)) , int(ysl::RoundUpDivide(zSize, blockedSize)) };
		//m_blockDimension = {int(xSize / blockedSize), int(ySize / blockedSize), int(zSize / blockedSize)};

		std::cout << g_xSize << " " << g_ySize << " " << g_zSize << std::endl;
		m_dataSize = m_blockDimension * ysl::Vector3i{ blockSize, blockSize, blockSize };

		m_step = blockSize - 2 * m_padding;

		std::cout << "Information:" << std::endl;
		std::cout << "block size:" << blockedSize << std::endl;
		std::cout << "lvd block dimension:" << m_blockDimension << std::endl;
		std::cout << "lvd data dimension: " << m_dataSize << std::endl;

		const auto rawBytes = std::size_t(xSize) * ySize * zSize * sizeof(RawType) + rawPointerOffset;

		if (rawBytes == 0)
		{
			throw std::runtime_error("empty data");
		}



		auto repo = ysl::LibraryReposity::GetLibraryRepo();
		assert(repo);
		repo->AddLibrary("ioplugin");

		rawIO = ysl::Object::CreateObject<IPluginFileMap>("common.filemapio");
		if (rawIO == nullptr)
			throw std::runtime_error("can not load ioplugin");

		rawIO->Open(fileName, rawBytes, FileAccess::Read, MapAccess::ReadOnly);

		//rawIO = std::make_shared<WindowsFileMapping>(fileName,
		//	rawBytes,
		//	WindowsFileMapping::Read, 
		//	WindowsFileMapping::ReadOnly);

		rawPtr = rawIO->FileMemPointer(0, rawBytes);
		if (!rawPtr)
		{
			throw std::runtime_error("raw file bad mapping");
		}
		const auto lvdBytes = size_t(m_dataSize.x) * size_t(m_dataSize.y) * size_t(m_dataSize.z) * sizeof(RawType);

		lvdIO = ysl::Object::CreateObject<IPluginFileMap>("common.filemapio");
		if (lvdIO == nullptr)
			throw std::runtime_error("can not load ioplugin");

		lvdIO->Open(outFileName, lvdBytes + LVD_HEADER_SIZE, FileAccess::ReadWrite, MapAccess::ReadWrite);
		lvdPtr = lvdIO->FileMemPointer(0, lvdBytes + LVD_HEADER_SIZE);
		if (!lvdPtr)
		{
			throw std::runtime_error("lvd file bad mapping");
		}
	}

	template <int nLogBlockSize, typename LVDTraits>
	bool RawToLVDConverter<nLogBlockSize, LVDTraits>::convert()
	{
		// Allocate a buffer for block data with padding 

		if (!lvdPtr)
		{
			std::cout << "Bad lvd data pointer\n";
			return false;
		}
		Timer timer;
		size_t curBlocks = 0;
		//std::atomic_size_t curBlocks = 0;

		auto const buf = lvdPtr + LVD_HEADER_SIZE;
		g_emptyValue = 0;
		const auto step = blockSize - 2 * m_padding;
		const auto totalBlocks = m_blockDimension.Prod();
		timer.start();
		//#pragma omp parallel for
		for (int zb = 0; zb < m_blockDimension.z; zb++)
		{
			
			for (int yb = 0; yb < m_blockDimension.y; yb++)
				for (int xb = 0; xb < m_blockDimension.x; xb++)
				{

					//	g_xOffset = -m_padding + xb * step;
					//	g_yOffset = -m_padding + yb * step;
					//	g_zOffset = -m_padding + zb * step;


					const int blockIndex = xb + yb * m_blockDimension.x + zb * m_blockDimension.x * m_blockDimension.y;
					//getData(buf + blockIndex * size_t(blockSize) * blockSize * blockSize,
					//	rawPtr + rawPointerOffset,
					//	blockSize,
					//	blockSize,
					//	blockSize,
					//	xb,
					//	yb,
					//	zb);
					GetData(buf + blockIndex * size_t(blockSize) * blockSize * blockSize,
						rawPtr + rawPointerOffset,
						g_xSize,
						g_ySize,
						g_zSize,
						blockSize,
						blockSize,
						blockSize,
						xb,
						yb,
						zb,
						-m_padding,
						-m_padding,
						-m_padding,
						m_step,
						m_step,
						m_step,
						0);
					++curBlocks;
					//curBlocks++;
					if (curBlocks % 100 == 0)
					{
						const float curPercent = curBlocks * 1.0 / totalBlocks;
						if (curPercent == 0.f)
							continue;
						const size_t seconds = timer.eval_remaining_time(curPercent) / 1000000.0;
						int hh = seconds / 3600;
						int mm = (seconds - hh * 3600) / 60;
						int ss = int(seconds) % 60;
						printf("%d of %d complete, make up %.2f%%. Estimated Remaining Time: %02d:%02d:%02d\r", curPercent, hh, mm, ss);
					}
				}
		}
		printf("\n");
		timer.stop();
		const size_t seconds = timer.duration() / 1000000.0;
		int hh = seconds / 3600;
		int mm = (seconds - hh * 3600) / 60;
		int ss = int(seconds) % 60;
		//printf("Convertion Finished. Total Time:%02d:%02d:%02d\n",hh,mm,ss);
		return true;
	}


	template <int nLogBlockSize, typename LVDTraits>
	bool RawToLVDConverter<nLogBlockSize, LVDTraits>::save(const std::string& fileName)
	{

		const auto rep = std::to_string(m_padding), bSize = std::to_string(1 << nLogBlockSize);
		const auto lvdBytes = size_t(m_dataSize.x) * size_t(m_dataSize.y) * size_t(m_dataSize.z) * sizeof(RawType);
		constexpr auto logBlockSize = nLogBlockSize;

		// 36 bytes in total

		LVDHeader lvdHeader;
		lvdHeader.magicNum = LVDTraits::MagicNumber;
		lvdHeader.dataDim[0] = m_dataSize.x;
		lvdHeader.dataDim[1] = m_dataSize.y;
		lvdHeader.dataDim[2] = m_dataSize.z;
		lvdHeader.padding = m_padding;
		lvdHeader.blockLengthInLog = logBlockSize;

		lvdHeader.originalDataDim[0] = m_blockDimension.x * ((1 << nLogBlockSize) - 2 * m_padding);
		lvdHeader.originalDataDim[1] = m_blockDimension.y * ((1 << nLogBlockSize) - 2 * m_padding);
		lvdHeader.originalDataDim[2] = m_blockDimension.z * ((1 << nLogBlockSize) - 2 * m_padding);

		const auto p = lvdHeader.Encode();

		std::cout << m_dataSize << " " <<
			logBlockSize << " " <<
			m_padding << " " <<
			g_xSize << " " <<
			g_ySize << " " <<
			g_zSize << std::endl;

		// Write lvd header
		memcpy(lvdPtr, p, LVD_HEADER_SIZE);

		std::cout << "writing .lvd file finished\n";
		return true;
	}



	template<int nLogBlockSize, typename LVDTraits = LVDFileTraits>
	class RawToLVDConverterEx
	{
		using RawType = unsigned char;
		//int g_xOffset, g_yOffset, g_zOffset;
		int g_xSize, g_ySize, g_zSize;
		int m_step;
		int g_xBlockSize, g_yBlockSize, g_zBlockSize;
		RawType g_emptyValue = 0;
		std::size_t rawPointerOffset;




		void getData(RawType* dest,
			const RawType* src,
			size_t width,
			size_t height,
			size_t depth,
			size_t xb,
			size_t yb,
			size_t zb) const;

		static void GetData(RawType * dest,
			const RawType * src, int width, int height, int depth,
			int w,
			int h,
			int d,
			int xb,
			int yb,
			int zb,
			int xbegin,
			int ybegin,
			int cbegin,
			int xstep,
			int ystep,
			int zstep,
			RawType fill);


		std::ifstream m_rawFile;
		std::ofstream m_lvdFile;


		ysl::Vector3i m_lvdDataSize;
		ysl::Vector3i m_blockDimension;

		std::unique_ptr<RawType[]> m_rawBuf;
		std::shared_ptr<ysl::IPluginFileMap> lvdIO;
		unsigned char * lvdPtr;
		const int m_padding;
		static constexpr auto blockSize = 1 << nLogBlockSize;
		std::string inFileName;
	public:
		RawToLVDConverterEx(const std::string& fileName, int xSize, int ySize, int zSize, int repeat, const std::string & outFileName, std::size_t offset = 0);
		auto DimensionInData()const { return m_lvdDataSize; }
		auto DimensionInBlock()const { return m_blockDimension; }
		void setBoundaryValue(RawType value) { g_emptyValue = value; }
		bool convert();
		bool save(const std::string& fileName);

	};

	template <int nLogBlockSize, typename LVDTraits>
	void RawToLVDConverterEx<nLogBlockSize, LVDTraits>::getData(RawType* dest,
		const RawType* src,
		size_t width,
		size_t height,
		size_t depth,
		size_t xb,
		size_t yb,
		size_t zb) const
	{
		//#pragma omp parallel for

		for (int z = 0; z < depth; z++)
			for (int y = 0; y < height; y++)
				for (int x = 0; x < width; x++)
				{
					const int xOffset = -m_padding + xb * m_step;
					const int yOffset = -m_padding + yb * m_step;
					const int zOffset = -m_padding + zb * m_step;
					const int gx = x + xOffset, gy = y + yOffset, gz = z + zOffset;
					const size_t blockIndex = x + y * g_xBlockSize + z * g_xBlockSize * g_yBlockSize;
					if (gx >= 0 && gx < g_xSize && gy >= 0 && gy < g_ySize && gz >= 0 && gz < g_zSize)
					{
						const size_t linearIndex = (gx)+(gy)* std::size_t(g_xSize) + (gz)* g_xSize * std::size_t(
							g_ySize);
						*(dest + blockIndex) = *(src + linearIndex);
					}
					else
					{
						*(dest + blockIndex) = g_emptyValue;
					}
				}
	}

	template <int nLogBlockSize, typename LVDTraits>
	void RawToLVDConverterEx<nLogBlockSize, LVDTraits>::GetData(RawType* dest,
		const RawType* src,
		int width,
		int height,
		int depth,
		int sw,
		int sh,
		int sd,

		int xb,
		int yb,
		int zb,

		int xbegin,
		int ybegin,
		int zbegin,

		int xstep,
		int ystep,
		int zstep,
		RawType fill)
	{
		for (int z = 0; z < sd; z++)
			for (int y = 0; y < sh; y++)
				for (int x = 0; x < sw; x++)
				{
					const int xOffset = xbegin + xb * xstep;
					const int yOffset = ybegin + yb * ystep;
					const int zOffset = zbegin + zb * zstep;
					const int gx = x + xOffset, gy = y + yOffset, gz = z + zOffset;
					const size_t blockIndex = x + y * (size_t)sw + z * (size_t)sw * (size_t)sh;
					if (gx >= 0 && gx < width && gy >= 0 && gy < height && gz >= 0 && gz < depth)
					{
						const size_t linearIndex = (gx)+(gy)* std::size_t(width) + (gz)* width * std::size_t(height);
						*(dest + blockIndex) = *(src + linearIndex);
					}
					else
					{
						*(dest + blockIndex) = fill;
					}
				}
	}


	template <int nLogBlockSize, typename LVDTraits>
	RawToLVDConverterEx<nLogBlockSize, LVDTraits>::RawToLVDConverterEx(const std::string& fileName, int xSize, int ySize,
		int zSize, int padding, const std::string & outFileName, std::size_t offset) :
		g_xSize(xSize),
		g_ySize(ySize),
		g_zSize(zSize),
		g_emptyValue(0),
		m_padding(padding),
		g_xBlockSize(blockSize),
		g_yBlockSize(blockSize),
		g_zBlockSize(blockSize),
		rawPointerOffset(offset),
		inFileName(fileName)
	{
		if (padding < 0 || padding > 2)
		{
			std::cout << "Unsupported repeat\n";
			return;
		}

		const auto blockedSize = blockSize - 2 * m_padding;
		m_blockDimension = { int(ysl::RoundUpDivide(xSize, blockedSize)), int(ysl::RoundUpDivide(ySize, blockedSize)) , int(ysl::RoundUpDivide(zSize, blockedSize)) };

		std::cout << g_xSize << " " << g_ySize << " " << g_zSize << std::endl;
		m_lvdDataSize = m_blockDimension * ysl::Vector3i{ blockSize, blockSize, blockSize };

		m_step = blockSize - 2 * m_padding;

		std::cout << "Information:" << std::endl;
		std::cout << "block size:" << blockedSize << std::endl;
		std::cout << "lvd block dimension:" << m_blockDimension << std::endl;
		std::cout << "lvd data dimension: " << m_lvdDataSize << std::endl;

		const auto rawBytes = std::size_t(xSize) * ySize * zSize * sizeof(RawType) + rawPointerOffset;

		if (rawBytes == 0)
		{
			throw std::runtime_error("empty data");
		}


		const auto lvdBytes = size_t(m_lvdDataSize.x) * size_t(m_lvdDataSize.y) * size_t(m_lvdDataSize.z) * sizeof(RawType);

		//auto repo = ysl::LibraryReposity::GetLibraryRepo();
		//assert(repo);
		//repo->AddLibrary("ioplugin");
		//lvdIO = ysl::Object::CreateObject<IPluginFileMap>("common.filemapio");
		//if (lvdIO == nullptr)
		//	throw std::runtime_error("can not load ioplugin");
		//lvdIO->Open(outFileName, lvdBytes + LVD_HEADER_SIZE, FileAccess::ReadWrite, MapAccess::ReadWrite);
		//lvdPtr = lvdIO->FileMemPointer(0, lvdBytes + LVD_HEADER_SIZE);
		//if (!lvdPtr)
		//{
		//	throw std::runtime_error("lvd file bad mapping");
		//}

		m_lvdFile.open(outFileName, std::ios::binary);
		if(!m_lvdFile.is_open())
		{
			throw std::runtime_error("can not open lvd file");
		}

	}

	template <int nLogBlockSize, typename LVDTraits>
	bool RawToLVDConverterEx<nLogBlockSize, LVDTraits>::convert()
	{

		RawReaderIO rawReader(inFileName, Size3(g_xSize, g_ySize, g_zSize), sizeof(RawType));

		struct Buffer
		{
			std::unique_ptr<RawType[]> buffer;
			Size3 size;
			int index;
			std::condition_variable cond_notify_read_compute;
			std::condition_variable cond_notify_write;
			std::mutex mtx;
			bool ready = false;
			Buffer(const Size3 & size, int index) :size(size), index(index)
			{
				buffer.reset(new RawType[size.Prod() * sizeof(RawType)]);
			}
		};

		Buffer readBuffer(Size3(g_xSize, g_ySize, blockSize), -1);
		Buffer writeBuffer(Size3(m_lvdDataSize.x, m_lvdDataSize.y, blockSize), -1);

		std::cout << "Total Read Task: " << m_blockDimension.z << std::endl;
		std::cout << "Total Write Task: " << m_blockDimension.z << std::endl;



		int coreNums = omp_get_num_procs();
		std::cout << "Core Numbers:" << coreNums << std::endl;

		Timer t;
		t.start();

		ThreadPool readThreadPool(1);

		//readThreadPool.Start();


		for (int i = 0; i < m_blockDimension.z; i++)
		{
			const int startz = (-m_padding + i * (blockSize - 2 * m_padding)) > 0 ? (-m_padding + i * (blockSize - 2 * m_padding)) : 0;
			const Vec3i start(0, 0, startz);

			const Size3 size(g_xSize, g_ySize,Clamp(g_zSize - startz, 0, blockSize));
			//const Size3 size(g_xSize, g_ySize,i!=0?Clamp(g_zSize - startz, 0, blockSize):62);
			//std::cout << size << std::endl;
			std::atomic_size_t totalBlocks = 0;
			readThreadPool.AppendTask([&rawReader,coreNums,
				start,			// start position of the whole raw data
				size,			// size of the sub region
				i,		        //task id
				this,
				&totalBlocks,
				&readBuffer,
				&writeBuffer]
				()
				{
					{
						std::unique_lock<std::mutex> lk(readBuffer.mtx);
						readBuffer.cond_notify_read_compute.wait(lk, [&readBuffer]() {return readBuffer.ready == false; });

						rawReader.readRegion(start, size, (unsigned char*)readBuffer.buffer.get());
						readBuffer.ready = true;		//flag


						std::unique_lock<std::mutex> lk2(writeBuffer.mtx);
						writeBuffer.cond_notify_read_compute.wait(lk2, [&writeBuffer]() {return writeBuffer.ready == false; });

						const int offset = (i == 0) ? -m_padding : 0;

#pragma omp parallel for
						for (int yb = 0; yb < m_blockDimension.y; yb++)
							for (int xb = 0; xb < m_blockDimension.x; xb++)
							{

								const int blockIndex = xb + yb * m_blockDimension.x;
								GetData(writeBuffer.buffer.get() + blockIndex * size_t(blockSize) * blockSize * blockSize,
									readBuffer.buffer.get(),
									size.x,
									size.y,
									size.z,
									blockSize,
									blockSize,
									blockSize,
									xb,
									yb,
									0,
									-m_padding,
									-m_padding,
									offset,
									m_step,
									m_step,
									m_step,
									0);
								++totalBlocks;
								printf("%10lld of %10lld complete.\r",totalBlocks.load(),m_blockDimension.Prod());
							}

						//Debug("Compute Task %d has been finished.", i);
						// compute finished
						readBuffer.ready = false;	// dirty, prepare for next read
						writeBuffer.index = i;
						writeBuffer.ready = true;	// ready to write
					}
					readBuffer.cond_notify_read_compute.notify_one();		// notify to read next section
					writeBuffer.cond_notify_write.notify_one();				// notify to the write thread to write into the disk

				});

			//std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}

		size_t totalBlocks = 0;
		ThreadPool writeThreadPool(1);
		//writeThreadPool.Start();
		for (int i = 0; i < m_blockDimension.z; i++)
		{
			writeThreadPool.AppendTask([this,&totalBlocks,&t,
				&writeBuffer]
				()
				{
					{
						std::unique_lock<std::mutex> lk(writeBuffer.mtx);
						writeBuffer.cond_notify_write.wait(lk, [&writeBuffer] {return writeBuffer.ready == true; });
						const auto bytes = (uint64_t)m_lvdDataSize.x * (uint64_t)m_lvdDataSize.y * (uint64_t)blockSize * sizeof(RawType);
						///TODO:: write to file
						m_lvdFile.seekp(LVD_HEADER_SIZE + writeBuffer.index*bytes, std::ios_base::beg);
						std::cout << "Write "<<bytes<<" Byte(s) To Disk:\n";
						Timer pt;
						pt.start();
						m_lvdFile.write((char*)writeBuffer.buffer.get(), bytes);
						pt.stop();
						std::cout << "Write To Disk Finished. Time:"<< pt.duration_to_seconds()<<"s.\n";
						totalBlocks += m_blockDimension.x*m_blockDimension.y;

						float curPercent = totalBlocks * 1.0 / m_blockDimension.Prod();
						size_t seconds = t.eval_remaining_time(curPercent) / 1000000;
						const int hh = seconds / 3600;
						const int mm = (seconds - hh * 3600) / 60;
						const int ss = int(seconds) % 60;
						printf("%20lld blocks finished, made up %.2f%%. Estimated remaining time: %02d:%02d:%02d\n", totalBlocks, totalBlocks*100.0 / m_blockDimension.Prod(),hh,mm,ss);
						writeBuffer.ready = false;		// prepare for next compute	
					}
					writeBuffer.cond_notify_read_compute.notify_one();			// notify to the read thread for the next read and computation
				});
			//std::this_thread::sleep_for(std::chrono::milliseconds(500));

		}

		//readThreadPool.Stop();
		//writeThreadPool.Stop();
		return true;
	}


	template <int nLogBlockSize, typename LVDTraits>
	bool RawToLVDConverterEx<nLogBlockSize, LVDTraits>::save(const std::string& fileName)
	{

		const auto rep = std::to_string(m_padding), bSize = std::to_string(1 << nLogBlockSize);
		const auto lvdBytes = size_t(m_lvdDataSize.x) * size_t(m_lvdDataSize.y) * size_t(m_lvdDataSize.z) * sizeof(RawType);
		constexpr auto logBlockSize = nLogBlockSize;

		// 36 bytes in total

		LVDHeader lvdHeader;
		lvdHeader.magicNum = LVDTraits::MagicNumber;
		lvdHeader.dataDim[0] = m_lvdDataSize.x;
		lvdHeader.dataDim[1] = m_lvdDataSize.y;
		lvdHeader.dataDim[2] = m_lvdDataSize.z;
		lvdHeader.padding = m_padding;
		lvdHeader.blockLengthInLog = logBlockSize;

		lvdHeader.originalDataDim[0] = m_blockDimension.x * ((1 << nLogBlockSize) - 2 * m_padding);
		lvdHeader.originalDataDim[1] = m_blockDimension.y * ((1 << nLogBlockSize) - 2 * m_padding);
		lvdHeader.originalDataDim[2] = m_blockDimension.z * ((1 << nLogBlockSize) - 2 * m_padding);

		const auto p = lvdHeader.Encode();

		std::cout << m_lvdDataSize << " " <<
			logBlockSize << " " <<
			m_padding << " " <<
			g_xSize << " " <<
			g_ySize << " " <<
			g_zSize << std::endl;

		// Write lvd header
		//auto header = lvdIO->FileMemPointer(0,LVD_HEADER_SIZE);
		//if (!header)
		//{
		//	throw std::runtime_error("lvd file bad mapping in save()");
		//}
		//memcpy(header, p, LVD_HEADER_SIZE);
		//lvdIO->DestroyFileMemPointer(header);
		m_lvdFile.seekp(0, std::ios_base::beg);
		m_lvdFile.write((char*)p, LVD_HEADER_SIZE);

		std::cout << "writing .lvd file finished\n";
		return true;
	}

}

#endif/*_LVDCONVERTER_H_*/
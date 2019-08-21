
#ifndef _LVDCONVERTER_H_
#define _LVDCONVERTER_H_

#include <cstring>
#include <fstream>
#include <tuple>
#include <atomic>

#include "geometry.h"
#include "numeric.h"
#include "lvdheader.h"
#include "rawio.h"
#include "libraryloader.h"
#include "timer.h"
#include "error.h"
#include "threadpool.h"
#include "rawreader.h"

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

template <int nLogBlockSize, typename LVDTraits = LVDFileTraits>
class RawToLVDConverterEx
{
	using RawType = unsigned char;
	//int g_xOffset, g_yOffset, g_zOffset;
	int g_xSize, g_ySize, g_zSize;
	int m_step;
	int g_xBlockSize, g_yBlockSize, g_zBlockSize;
	RawType g_emptyValue = 0;
	std::size_t rawPointerOffset;

	std::ifstream m_rawFile;
	std::ofstream m_lvdFile;

	ysl::Vector3i m_lvdDataSize;
	ysl::Vector3i m_blockDimension;

	std::unique_ptr<RawType[]> m_rawBuf;
	std::shared_ptr<ysl::IPluginFileMap> lvdIO;
	unsigned char *lvdPtr;
	const int m_padding;
	static constexpr auto blockSize = 1 << nLogBlockSize;
	std::string inFileName;

public:
	RawToLVDConverterEx( const std::string &fileName,
						 int xSize,
						 int ySize,
						 int zSize,
						 int padding,
						 const std::string &outFileName,
						 std::size_t offset = 0 );
	auto DimensionInData() const { return m_lvdDataSize; }
	auto DimensionInBlock() const { return m_blockDimension; }
	void setBoundaryValue( RawType value ) { g_emptyValue = value; }
	bool convert( std::size_t suggestMemGb = 128 );
	bool save();
};

template <int nLogBlockSize, typename LVDTraits>
RawToLVDConverterEx<nLogBlockSize, LVDTraits>::RawToLVDConverterEx( const std::string &fileName,
																	int xSize,
																	int ySize,
																	int zSize,
																	int padding,
																	const std::string &outFileName,
																	std::size_t offset ) :
  g_xSize( xSize ),
  g_ySize( ySize ),
  g_zSize( zSize ),
  g_emptyValue( 0 ),
  m_padding( padding ),
  g_xBlockSize( blockSize ),
  g_yBlockSize( blockSize ),
  g_zBlockSize( blockSize ),
  rawPointerOffset( offset ),
  inFileName( fileName )
{
	if ( padding < 0 || padding > 2 ) {
		std::cout << "Unsupported padding\n";
		return;
	}

	m_step = blockSize - 2 * m_padding;
	m_blockDimension = { int( ysl::RoundUpDivide( xSize, m_step ) ), int( ysl::RoundUpDivide( ySize, m_step ) ), int( ysl::RoundUpDivide( zSize, m_step ) ) };

	std::cout << g_xSize << " " << g_ySize << " " << g_zSize << std::endl;
	m_lvdDataSize = m_blockDimension * ysl::Vector3i{ blockSize, blockSize, blockSize };

	std::cout << "Information: " << std::endl;
	std::cout << "inner block size: "
			  << blockSize << " - 2 x "
			  << m_padding << " = " << m_step << std::endl;
	std::cout << "lvd block dimension: " << m_blockDimension << std::endl;
	std::cout << "lvd data dimension: " << m_lvdDataSize << std::endl;

	const auto rawBytes = std::size_t( xSize ) * ySize * zSize * sizeof( RawType ) + rawPointerOffset;

	if ( rawBytes == 0 ) {
		throw std::runtime_error( "empty data" );
	}

	const auto lvdBytes = size_t( m_lvdDataSize.x ) * size_t( m_lvdDataSize.y ) * size_t( m_lvdDataSize.z ) * sizeof( RawType );

	m_lvdFile.open( outFileName, std::ios::binary );
	if ( !m_lvdFile.is_open() ) {
		throw std::runtime_error( "can not open lvd file" );
	}
}

template <int nLogBlockSize, typename LVDTraits>
bool RawToLVDConverterEx<nLogBlockSize, LVDTraits>::convert( std::size_t suggestMemGb )
{
	RawReaderIO rawReader( inFileName, Size3( g_xSize, g_ySize, g_zSize ), sizeof( RawType ) );

	struct Buffer
	{
		std::unique_ptr<RawType[]> buffer;
		std::pair<int, std::size_t> stride;
		std::condition_variable cond_notify_read_compute;
		std::condition_variable cond_notify_write;
		std::mutex mtx;
		bool ready = false;
		Buffer( std::size_t nVoxels )
		{
			buffer.reset( new RawType[ nVoxels * sizeof( RawType ) ] );
		}
	};

	const std::size_t nVoxelsPerBlock = std::size_t( blockSize ) * blockSize * blockSize;

	const auto [ nCols, nRows, nSlices ] = m_blockDimension;

	const auto [ nColsPerStride, nRowsPerStride, strideInterval ] = [&] {
		std::size_t gbToBytes = std::size_t( 1024 ) /*Mb*/ * 1024 /*Kb*/ * 1024 /*Bytes*/;
		std::size_t blockSizeInBytes = sizeof( RawType ) * nVoxelsPerBlock;
		std::size_t memSizeInBytes = suggestMemGb * gbToBytes;
		int nBlocksInMem = memSizeInBytes / blockSizeInBytes / 2 /*two buffers*/;
		if ( not nBlocksInMem ) {
			throw std::runtime_error( "total memory < block size" );
		}
		// const int maxBlocksPerStride = 2;
		// nBlocksInMem = std::min( nBlocksInMem, maxBlocksPerStride );
		int nRowsPerStride = std::min( nBlocksInMem / nCols, nRows );
		int nColsPerStride = nCols;
		int strideInterval = 1;
		if ( not nRowsPerStride ) { /*nBlocksInMem < nBlocksPerRow*/
			nRowsPerStride = 1;
			nColsPerStride = nBlocksInMem;
			strideInterval = ysl::RoundUpDivide( nCols, nColsPerStride );
		}
		return std::make_tuple( nColsPerStride, nRowsPerStride, strideInterval );
	}();
	const int nBlocksPerStride = nColsPerStride * nRowsPerStride;
	std::cout << "Stride Size: "
			  << nColsPerStride << " x "
			  << nRowsPerStride << " = "
			  << nBlocksPerStride << " Blocks(s)" << std::endl;

	const int nRowIters =
	  ysl::RoundUpDivide( nRows, nRowsPerStride );

	std::cout << "Total Strides: "
			  << nRowIters * strideInterval * nSlices << std::endl;

	// since readBuffer is no larger than writeBuffer
	const std::size_t bufferSize = nVoxelsPerBlock * nBlocksPerStride;
	std::cout << "Allocing Buffers: "
			  << bufferSize
			  << " x 2 Byte(s) = "
			  << bufferSize / 1024 /*Kb*/ / 1024 /*Mb*/
			  << " Mb" << std::endl;
	Buffer readBuffer( nVoxelsPerBlock * nBlocksPerStride );
	Buffer writeBuffer( nVoxelsPerBlock * nBlocksPerStride );

	Timer t;
	t.start();

	ThreadPool readThreadPool( 1 );
	ThreadPool writeThreadPool( 1 );

	std::size_t totalReadBlocks = 0;
	std::size_t totalWriteBlocks = 0;

	auto readTask = [&, this]( int slice, int it, int rep ) {
		const Vec2i strideStart( rep * nColsPerStride, it * nRowsPerStride );
		const Size2 strideSize(
		  std::min( nCols - strideStart.x, nColsPerStride ),
		  std::min( nRows - strideStart.y, nRowsPerStride ) );

		/* left bottom corner of region, if padding > 0 this coord might be < 0 */
		Vec3i regionStart(
		  strideStart.x * m_step - m_padding,
		  strideStart.y * m_step - m_padding,
		  std::max( -m_padding + slice * m_step, 0 ) );
		/* whole region size includes padding, might overflow */
		Size3 regionSize(
		  strideSize.x * m_step + m_padding * 2,
		  strideSize.y * m_step + m_padding * 2,
		  Clamp( g_zSize - regionStart.z, 0, blockSize ) );
		auto rawRegionSize = regionSize;

		/* overflow = bbbb -> -x,x,-y,y */
		int overflow = 0;
		/* region overflows: x1 > X */
		if ( regionSize.x + regionStart.x > g_xSize ) {
			regionSize.x = g_xSize - regionStart.x;
			overflow |= 0b0100;
		}
		/* region overflows: y1 > Y */
		if ( regionSize.y + regionStart.y > g_ySize ) {
			regionSize.y = g_ySize - regionStart.y;
			overflow |= 0b0001;
		}
		/* region overflows: x0 < 0, must be a padding */
		if ( regionStart.x < 0 ) {
			regionSize.x -= m_padding;
			regionStart.x = 0;
			overflow |= 0b1000;
		}
		/* region overflows: y0 < 0, must be a padding */
		if ( regionStart.y < 0 ) {
			regionSize.y -= m_padding;
			regionStart.y = 0;
			overflow |= 0b0010;
		}

		std::size_t dx = 0, dy = 0;
		/* let x_i = first voxel of line i
		   x_i = len * i
		   -> x'_i = slen * i + dx? * 1 + slen * dy?*/
		if ( overflow & 0b1000 ) {
			dx = m_padding;
		}
		if ( overflow & 0b0010 ) {
			dy = m_padding;
		}

		const int zoffset = ( slice == 0 ) ? -m_padding : 0;

		{
			std::unique_lock<std::mutex> lk( readBuffer.mtx );
			readBuffer.cond_notify_read_compute.wait(
			  lk, [&] { return not readBuffer.ready; } );

			std::cout << "Read Stride: " << strideStart << " " << strideSize << std::endl;
			std::cout << "Read Region: " << regionStart << " " << regionSize << std::endl;

			/* always read region into buffer[0..] */
			rawReader.readRegion(
			  regionStart, regionSize, (unsigned char *)readBuffer.buffer.get() );
			readBuffer.ready = true;  //flag

			std::unique_lock<std::mutex> lk2( writeBuffer.mtx );
			writeBuffer.cond_notify_read_compute.wait(
			  lk2, [&] { return not writeBuffer.ready; } );

			/* transfer overflowed into correct position */
			if ( overflow ) {
				memset( writeBuffer.buffer.get(), 0, sizeof( RawType ) * rawRegionSize.Prod() );
				for ( int i = regionSize.y - 1; i >= 0; --i ) {
					memcpy(
					  writeBuffer.buffer.get() + rawRegionSize.x * ( i + dy ) + dx, /*x'_i*/
					  readBuffer.buffer.get() + regionSize.x * i,					/*x_i*/
					  regionSize.x * sizeof( RawType ) );
				}
				writeBuffer.buffer.swap( readBuffer.buffer );
			}

#           pragma omp parallel for
			for ( int yb = 0; yb < strideSize.y; yb++ ) {
				for ( int xb = 0; xb < strideSize.x; xb++ ) {
					const int blockIndex = xb + yb * strideSize.x;
					const auto destPtr = writeBuffer.buffer.get() +
										 blockIndex * nVoxelsPerBlock;
					const auto srcPtr = readBuffer.buffer.get() +
										xb * m_step + yb * m_step * rawRegionSize.x;

					for ( int row = 0; row < blockSize; ++row ) {
						memcpy(
						  destPtr + row * blockSize,
						  srcPtr + row * rawRegionSize.x,
						  blockSize * sizeof( RawType ) );
					}
					// z-offset

					++totalReadBlocks;
					printf( "%10lld of %10lld complete.\r", totalReadBlocks, m_blockDimension.Prod() );
				}
			}

			// compute finished
			readBuffer.ready = false;  // dirty, prepare for next read
			writeBuffer.stride = std::make_pair(
			  slice * m_blockDimension.x * m_blockDimension.y +
				it * nCols * nRowsPerStride +
				rep * nColsPerStride,
			  strideSize.Prod() );
			writeBuffer.ready = true;  // ready to write
		}

		readBuffer.cond_notify_read_compute.notify_one();  // notify to read next section
		writeBuffer.cond_notify_write.notify_one();		   // notify to the write thread to write into the disk
	};

	auto writeTask = [&, this] {
		{
			std::unique_lock<std::mutex> lk( writeBuffer.mtx );
			writeBuffer.cond_notify_write.wait( lk, [&] { return writeBuffer.ready; } );

			const auto [ index, nBlocks ] = writeBuffer.stride;

			const auto oneBlock = nVoxelsPerBlock * sizeof( RawType );
			const auto offset = oneBlock * index;
			const auto nBytes = oneBlock * nBlocks;
			///TODO:: write to file
			m_lvdFile.seekp( LVD_HEADER_SIZE + offset, std::ios_base::beg );
			std::cout << "Write " << nBytes << " Byte(s) To Disk:\n";
			Timer pt;
			pt.start();
			m_lvdFile.write( (char *)writeBuffer.buffer.get(), nBytes );
			pt.stop();
			std::cout << "Write To Disk Finished. Time:" << pt.duration_to_seconds() << "s.\n";
			totalWriteBlocks += m_blockDimension.x * m_blockDimension.y;

			float curPercent = totalWriteBlocks * 1.0 / m_blockDimension.Prod();
			size_t seconds = t.eval_remaining_time( curPercent ) / 1000000;
			const int hh = seconds / 3600;
			const int mm = ( seconds - hh * 3600 ) / 60;
			const int ss = int( seconds ) % 60;
			printf( "%20lld blocks finished, made up %.2f%%. Estimated remaining time: %02d:%02d:%02d\n",
					totalWriteBlocks, totalWriteBlocks * 100.0 / m_blockDimension.Prod(), hh, mm, ss );
			writeBuffer.ready = false;  // prepare for next compute
		}
		writeBuffer.cond_notify_read_compute.notify_one();  // notify to the read thread for the next read and computation
	};

	for ( int slice = 0; slice < nSlices; slice++ ) {
		for ( int it = 0; it < nRowIters; ++it ) {
			for ( int rep = 0; rep < strideInterval; ++rep ) {
				readThreadPool.AppendTask( readTask, slice, it, rep );
				writeThreadPool.AppendTask( writeTask );
			}
		}
	}

	return true;
}

template <int nLogBlockSize, typename LVDTraits>
bool RawToLVDConverterEx<nLogBlockSize, LVDTraits>::save()
{
	const auto rep = std::to_string( m_padding ), bSize = std::to_string( 1 << nLogBlockSize );
	const auto lvdBytes = size_t( m_lvdDataSize.x ) * size_t( m_lvdDataSize.y ) * size_t( m_lvdDataSize.z ) * sizeof( RawType );
	constexpr auto logBlockSize = nLogBlockSize;

	// 36 bytes in total
	LVDHeader lvdHeader;
	lvdHeader.magicNum = LVDTraits::MagicNumber;
	lvdHeader.dataDim[ 0 ] = m_lvdDataSize.x;
	lvdHeader.dataDim[ 1 ] = m_lvdDataSize.y;
	lvdHeader.dataDim[ 2 ] = m_lvdDataSize.z;
	lvdHeader.padding = m_padding;
	lvdHeader.blockLengthInLog = logBlockSize;

	lvdHeader.originalDataDim[ 0 ] = g_xSize;
	lvdHeader.originalDataDim[ 1 ] = g_ySize;
	lvdHeader.originalDataDim[ 2 ] = g_zSize;

	const auto p = lvdHeader.Encode();

	std::cout << m_lvdDataSize << " " << logBlockSize << " " << m_padding << " " << g_xSize << " " << g_ySize << " " << g_zSize << std::endl;

	m_lvdFile.seekp( 0, std::ios_base::beg );
	m_lvdFile.write( (char *)p, LVD_HEADER_SIZE );

	std::cout << "writing .lvd file finished\n";
	return true;
}

}  // namespace ysl

#endif /*_LVDCONVERTER_H_*/

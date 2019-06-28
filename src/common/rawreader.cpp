
#include "rawreader.h"
#include "rawio.h"
#include <cstring> // memcpy
#include "libraryloader.h"
#include <cassert>
#include <iostream>


namespace ysl
{

	RawReader::RawReader(const std::string &fileName, const ysl::Size3 &dimensions, size_t voxelSize)
		: fileName(fileName), dimensions(dimensions), voxelSize(voxelSize)//,file(fileName,std::ios::binary),
		, offset(0), ptr(nullptr)
	{
		const auto rawBytes = dimensions.x * dimensions.y * dimensions.z * voxelSize;
		auto repo = LibraryReposity::GetLibraryRepo();
		assert(repo);
		repo->AddLibrary("ioplugin");
		io = Object::CreateObject<IPluginFileMap>("common.filemapio");
		if (io == nullptr)
			throw std::runtime_error("can not load ioplugin");
		io->Open(fileName, rawBytes, FileAccess::Read, MapAccess::ReadOnly);
		ptr = io->FileMemPointer(0, rawBytes);
		if (!ptr)
		{
			throw std::runtime_error("map file failed RawReader::RawReader");
		}
	}

	RawReader::~RawReader()
	{
		io->DestroyFileMemPointer(ptr);
		io->Close();
	}
	// Read a region of volume data from the file into the buffer passed. It's assumed
	// the buffer passed has enough room. Returns the number voxels read
	size_t RawReader::readRegion(const ysl::Size3 &start, const ysl::Size3 &size, unsigned char *buffer)
	{
		seekAmt = 0;
		return readRegion__(start, size, buffer);
	}

	std::size_t RawReader::readRegion__(const ysl::Size3& start, const ysl::Size3& size, unsigned char* buffer)
	{
		assert(size.x > 0 && size.y > 0 && size.z > 0);
		const uint64_t startRead = (start.x + dimensions.x * (start.y + dimensions.y * start.z)) * voxelSize;
		if (offset != startRead)
		{
			/*	seekAmt += startRead - offset;
				std::cout << seekAmt<< std::endl;

				seekAmt = startRead - offset;
				if (!file.seekg(seekAmt, std::ios_base::cur))
				{
					throw std::runtime_error("ImportRAW: Error seeking file");
				}

				if (fseek(file, seekAmt, SEEK_CUR) != 0)
				{
					throw std::runtime_error("ImportRAW: Error seeking file");
				}*/

			offset = startRead;
		}

		// Figure out how to actually read the region since it may not be a full X/Y slice and
		// we'll need to read the portions in X & Y and seek around to skip regions
		size_t read = 0;
		if (convexRead(size))		// continuous read 
		{
			//read = fread(buffer, voxelSize, size.x * size.y * size.z, file);

			//file.read(reinterpret_cast<char*>(buffer), voxelSize* size.x * size.y * size.z);
			//read = file.gcount()/voxelSize;

			memcpy(reinterpret_cast<void*>(buffer), ptr + offset, voxelSize* size.x * size.y * size.z);

			read = size.x * size.y * size.z;	// voxel count

			offset = startRead + read * voxelSize;
		}
		else if (size.x == dimensions.x) {		// read by slice
			for (auto z = start.z; z < start.z + size.z; ++z) {
				const ysl::Size3 startSlice(start.x, start.y, z);
				const ysl::Size3 sizeSlice(size.x, size.y, 1);
				read += readRegion__(startSlice, sizeSlice, buffer + read * voxelSize);
			}
		}
		else {
			for (auto z = start.z; z < start.z + size.z; ++z) {
				for (auto y = start.y; y < start.y + size.y; ++y) {
					const ysl::Size3 startLine(start.x, y, z);
					const ysl::Size3 sizeLine(size.x, 1, 1);
					read += readRegion__(startLine, sizeLine, buffer + read * voxelSize);
				}
			}
		}
		return read;
	}

	RawReaderIO::RawReaderIO(const std::string& fileName, const ysl::Size3& dimensions, size_t voxelSize):fileName(fileName), dimensions(dimensions), voxelSize(voxelSize)//,file(fileName,std::ios::binary),
		, offset(0), ptr(nullptr)
	{
		const auto rawBytes = dimensions.x * dimensions.y * dimensions.z * voxelSize;
		file.open(fileName, std::ios::binary);
		if(!file.is_open())
		{
			throw std::runtime_error("RawReaderIO::RawReaderIO()::can not open file");
		}
	}

	RawReaderIO::~RawReaderIO()
	{

	}

	size_t RawReaderIO::readRegion(const ysl::Size3& start, const ysl::Size3& size, unsigned char* buffer)
	{
		seekAmt = 0;
		return readRegion__(start, size, buffer);
	}

	std::size_t RawReaderIO::readRegion__(const ysl::Size3& start, const ysl::Size3& size, unsigned char* buffer)
	{
		assert(size.x > 0 && size.y > 0 && size.z > 0);
		const uint64_t startRead = (start.x + dimensions.x * (start.y + dimensions.y * start.z)) * voxelSize;
		if (offset != startRead)
		{
			seekAmt += startRead - offset;
			//std::cout << seekAmt << std::endl;

			seekAmt = startRead - offset;
			if (!file.seekg(seekAmt, std::ios_base::cur))
			{
				throw std::runtime_error("ImportRAW: Error seeking file");
			}

			//if (fseek(file, seekAmt, SEEK_CUR) != 0)
			//{
			//	throw std::runtime_error("ImportRAW: Error seeking file");
			//}

			offset = startRead;
		}

		// Figure out how to actually read the region since it may not be a full X/Y slice and
		// we'll need to read the portions in X & Y and seek around to skip regions
		size_t read = 0;
		if (convexRead(size))		// continuous read 
		{
			//read = fread(buffer, voxelSize, size.x * size.y * size.z, file);

			file.read(reinterpret_cast<char*>(buffer), voxelSize* size.x * size.y * size.z);
			read = file.gcount()/voxelSize;

			//memcpy(reinterpret_cast<void*>(buffer), ptr + offset, voxelSize* size.x * size.y * size.z);

			read = size.x * size.y * size.z;	// voxel count

			offset = startRead + read * voxelSize;
		}
		else if (size.x == dimensions.x) {		// read by slice
			for (auto z = start.z; z < start.z + size.z; ++z) {
				const ysl::Size3 startSlice(start.x, start.y, z);
				const ysl::Size3 sizeSlice(size.x, size.y, 1);
				read += readRegion__(startSlice, sizeSlice, buffer + read * voxelSize);
			}
		}
		else {
			for (auto z = start.z; z < start.z + size.z; ++z) {
				for (auto y = start.y; y < start.y + size.y; ++y) {
					const ysl::Size3 startLine(start.x, y, z);
					const ysl::Size3 sizeLine(size.x, 1, 1);
					read += readRegion__(startLine, sizeLine, buffer + read * voxelSize);
				}
			}
		}
		return read;
	}
}

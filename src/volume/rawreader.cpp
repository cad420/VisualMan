
#include "rawreader.h"
#include "../mathematics/geometry.h"



namespace ysl
{

	RawReader::RawReader(const std::string &fileName, const ysl::Size3 &dimensions, size_t voxelSize)
		: fileName(fileName), dimensions(dimensions), voxelSize(voxelSize),
		file(fileName, std::ios::binary), offset(0)
	{
		if (!file)
		{
			throw std::runtime_error("ImportRAW: Unable to open file " + fileName);
		}
	}
	RawReader::~RawReader() {

	}
	// Read a region of volume data from the file into the buffer passed. It's assumed
	// the buffer passed has enough room. Returns the number voxels read
	size_t RawReader::readRegion(const ysl::Size3 &start, const ysl::Size3 &size, unsigned char *buffer) {
		assert(size.x > 0 && size.y > 0 && size.z > 0);

		const int64_t startRead = (start.x + dimensions.x * (start.y + dimensions.y * start.z)) * voxelSize;
		if (offset != startRead) {
			const int64_t seekAmt = startRead - offset;
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
		if (convexRead(size)) {
			//read = fread(buffer, voxelSize, size.x * size.y * size.z, file);

			file.read(reinterpret_cast<char*>(buffer), voxelSize* size.x * size.y * size.z);
			read = file.gcount();

			offset = startRead + read * voxelSize;
		}
		else if (size.x == dimensions.x) {
			for (auto z = start.z; z < start.z + size.z; ++z) {
				const ysl::Size3 startSlice(start.x, start.y, z);
				const ysl::Size3 sizeSlice(size.x, size.y, 1);
				read += readRegion(startSlice, sizeSlice, buffer + read * voxelSize);
			}
		}
		else {
			for (auto z = start.z; z < start.z + size.z; ++z) {
				for (auto y = start.y; y < start.y + size.y; ++y) {
					const ysl::Size3 startLine(start.x, y, z);
					const ysl::Size3 sizeLine(size.x, 1, 1);
					read += readRegion(startLine, sizeLine, buffer + read * voxelSize);
				}
			}
		}
		return read;
	}



}

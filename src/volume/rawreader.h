
#ifndef _RAWREADER_H_
#define _RAWREADER_H_
#include "../mathematics/geometry.h"
#include <fstream>


namespace ysl
{

	class RawReader
	{
		std::string fileName;
		ysl::Size3 dimensions;
		size_t voxelSize;
		//FILE * file;
		int64_t offset;
		std::ifstream file;

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
}
#endif
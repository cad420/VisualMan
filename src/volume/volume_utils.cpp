
#include "volume_utils.h"

#include <exception>

RawReader::RawReader(const std::string &fileName, const ysl::Size3 &dimensions, size_t voxelSize)
	: fileName(fileName), dimensions(dimensions), voxelSize(voxelSize),
	file(fopen(fileName.c_str(), "rb")), offset(0)
{
	if (!file) 
	{
		throw std::runtime_error("ImportRAW: Unable to open file " + fileName);
	}
}
RawReader::~RawReader() {
	fclose(file);
}
// Read a region of volume data from the file into the buffer passed. It's assumed
// the buffer passed has enough room. Returns the number voxels read
size_t RawReader::readRegion(const ysl::Size3 &start, const ysl::Size3 &size, unsigned char *buffer) {
	assert(size.x > 0 && size.y > 0 && size.z > 0);

	const int64_t startRead = (start.x + dimensions.x * (start.y + dimensions.y * start.z)) * voxelSize;
	if (offset != startRead) {
		const int64_t seekAmt = startRead - offset;
		if (fseek(file, seekAmt, SEEK_CUR) != 0) {
			throw std::runtime_error("ImportRAW: Error seeking file");
		}
		offset = startRead;
	}

	// Figure out how to actually read the region since it may not be a full X/Y slice and
	// we'll need to read the portions in X & Y and seek around to skip regions
	size_t read = 0;
	if (convexRead(size)) {
		read = fread(buffer, voxelSize, size.x * size.y * size.z, file);
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




LVDReader::LVDReader(const std::string& fileName): validFlag(true)
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

	int vx, vy, vz, bx, by, bz;
	int m_originalWidth, m_originalHeight, m_originalDepth;

	fileHandle.read((char*)&vx, sizeof(int));
	fileHandle.read((char*)&vy, sizeof(int));
	fileHandle.read((char*)&vz, sizeof(int));
	fileHandle.read((char*)&logBlockSize, sizeof(int));
	fileHandle.read((char*)&repeat, sizeof(int));
	fileHandle.read((char*)&m_originalWidth, sizeof(int));
	fileHandle.read((char*)&m_originalHeight, sizeof(int));
	fileHandle.read((char*)&m_originalDepth, sizeof(int));

	if (logBlockSize != LogBlockSize5 && logBlockSize != LogBlockSize6)
	{
		std::cout << "Unsupported block size\n";
		validFlag = false;
		return;
	}

	const size_t aBlockSize = BlockSize();

	// aBlockSize must be power of 2, e.g. 32 or 64
	bx = ((vx + aBlockSize - 1) & ~(aBlockSize - 1)) / aBlockSize;
	by = ((vy + aBlockSize - 1) & ~(aBlockSize - 1)) / aBlockSize;
	bz = ((vz + aBlockSize - 1) & ~(aBlockSize - 1)) / aBlockSize;

	vSize = ysl::Size3((vx),(vy),(vz));
	bSize = ysl::Size3(bx,by,bz);
	oSize = ysl::Size3(m_originalWidth,m_originalHeight,m_originalDepth);

}

void LVDReader::ReadBlock(char * dest, int blockId)
{
	const size_t blockCount = BlockDataCount();
	// 32 is the size of file header
	fileHandle.seekg(blockCount * blockId + 36, std::ios::beg);
	fileHandle.read(dest, sizeof(char) * blockCount);
}

namespace  ysl
{

	void TransferFunction::read(const std::string& fileName)
	{
		FILE* fp = fopen(fileName.c_str(), "r");
		if (!fp)
		{
			m_valid = false;
			return;
		}
		int keyNum;
		fscanf(fp, "%d %f %f\n", &keyNum, &m_leftThreshold, &m_rightThreshold);

		m_tfKeys.reserve(keyNum);

		float intensity;
		int rl, gl, bl, al, rr, gr, br, ar;
		Float rgba1[4], rgba2[4];
		for (auto i = 0; i < keyNum; ++i)
		{
			fscanf(fp, "%f %d %d %d %d %d %d %d %d\n", &intensity,
				&rl, &gl, &bl, &al, &rr, &gr, &br, &ar);
			rgba1[0] = rl / 255.0f;
			rgba1[1] = gl / 255.0f;
			rgba1[2] = bl / 255.0f;
			rgba1[3] = al / 255.0f;
			rgba2[0] = rr / 255.0f;
			rgba2[1] = br / 255.0f;
			rgba2[2] = gr / 255.0f;
			rgba2[3] = ar / 255.0f;
			m_tfKeys.emplace_back(intensity, ysl::RGBASpectrum{ rgba1 }, ysl::RGBASpectrum{ rgba2 });
		}
		m_valid = true;
		fclose(fp);
	}

	void TransferFunction::FetchData(RGBASpectrum* transferFunction, int dimension)
	{
		const auto frontEnd = static_cast<int>(std::floor(m_leftThreshold * dimension + 0.5));
		const auto backStart = static_cast<int>(std::floor(m_rightThreshold * dimension + 0.5));
		//all values before front_end and after back_start are set to zero
		//all other values remain the same
		for (int x = 0; x < frontEnd; ++x)
			transferFunction[x] = RGBASpectrum{0.f};

		auto keyIterator = m_tfKeys.begin();
		RGBASpectrum color;
		for (auto x = frontEnd; x < backStart; ++x)
		{
			const auto value = static_cast<float>(x) / (dimension - 1);
			while ((keyIterator != m_tfKeys.end()) && (value > (*keyIterator).Intensity()))
				++keyIterator;
			if (keyIterator == m_tfKeys.begin())
			{
				color = m_tfKeys[0].LeftColor();
			}
			else if (keyIterator == m_tfKeys.end())
			{
				color = (*(keyIterator - 1)).RightColor();
			}
			else
			{
				// calculate the value weighted by the destination to the next left and right key
				const auto& leftKey = *(keyIterator - 1);
				const auto& rightKey = *keyIterator;
				const auto fraction = (value - leftKey.Intensity()) / (rightKey.Intensity() - leftKey.Intensity());
				const auto leftDest = leftKey.RightColor();
				const auto rightDest = rightKey.LeftColor();
				color = leftDest + (rightDest - leftDest) * fraction;
			}
			//if (factor != 1)
			//	a = 1.0 - pow(1.f - a, factor);
			transferFunction[x] = color;
		}
		for (int x = backStart; x < dimension; ++x)
			transferFunction[x] = RGBASpectrum{0.f};
	}
}

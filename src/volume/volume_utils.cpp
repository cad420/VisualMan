
#include "volume_utils.h"

BlockVolumeReader::BlockVolumeReader(const std::string& fileName): validFlag(true)
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
	fileHandle.read((char*)&m_repeat, sizeof(int));
	if (logBlockSize != LogBlockSize5)
	{
		std::cout << "Unsupported block size\n";
		validFlag = false;
		return;
	}
	const size_t ablockSize = blockSize();
	bx = ((vx + ablockSize - 1) & ~(ablockSize - 1)) / ablockSize;
	by = ((vy + ablockSize - 1) & ~(ablockSize - 1)) / ablockSize;
	bz = ((vz + ablockSize - 1) & ~(ablockSize - 1)) / ablockSize;
}

void BlockVolumeReader::readBlock(char* dest, int blockId)
{
	const size_t blockCount = blockDataCount();
	fileHandle.seekg(blockCount * blockId, std::ios::beg);
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


#include "../src/volume/volume_utils.h"
#include <iomanip>
#include "../src/volume/rawreader.h"
#ifndef _DOWNSAMPLE_H_
#define _DOWNSAMPLE_H_


int main(int argc, char* argv[])
{
	//std::string fileName;
	//std::cin >> fileName;

	//LVDReader reader(fileName);
	//int blockIndex;

	//const auto blockDim = reader.SizeByBlock();
	//std::cout << "BlockDim:" << blockDim << std::endl;

	//const auto bytes = reader.BlockDataCount();
	//std::unique_ptr<unsigned char[]> buf(new unsigned char[bytes]);

	//const auto blockSize = reader.BlockSize();
	//std::cout << "Block Size:" << blockSize << " " << blockSize << " " << blockSize << std::endl;

	//const auto rep = reader.BoundaryRepeat();

	//std::vector<int> hist(256);

	//int bCount = reader.BlockCount();
	//for (int i = 0; i < bCount; i++)
	//{
	//	reader.ReadBlock((char*)buf.get(), i);
	//	for (int z = 0; z < blockSize; z++)
	//	{
	//		for(int y = 0 ;y<blockSize;y++)
	//		{
	//			for (int x = 0; x < blockSize; x++)
	//			{
	//				if (x >= rep && x < blockSize - rep && y >= rep && y < blockSize - rep && z>=rep && z<blockSize-rep)
	//				{
	//					const auto voxelIndex = ysl::Linear(ysl::Point3i(x, y, z), ysl::Size2(blockSize, blockSize));
	//					hist[buf[voxelIndex]]++;
	//				}

	//			}
	//		}
	//	}
	//	std::cout << i << std::endl;
	//}

	//const auto s = reader.OriginalDataSize();
	//const auto totals = s.x*s.y*s.z;
	//int i = 0;
	//double b = 0;
	//for(const auto v:hist)
	//{
	//	b += 1.0*v / totals;
	//	std::cout << i++ << ": " << v << ", "<<std::fixed<<std::setprecision(2) << b << std::endl;
	//}

	const std::string fileName = R"(D:\scidata\abc\s1_480_480_480.raw)";
	const ysl::Size3 size{480,480,480};

	const std::string outFileName = R"(D:\Desktop\test.raw)";
	const ysl::Size3 bufSize{480,480,480};

	std::unique_ptr<unsigned char[]> buf(new unsigned char[bufSize.x*bufSize.y*bufSize.z]);


	ysl::RawReader reader(fileName, size,1);
	reader.readRegion({0,0,0}, bufSize, buf.get());

	//ysl::WindowsMappingRawIO io(fileName, bufSize, 1);


	std::ofstream outFile(outFileName, std::ios::binary);
	outFile.write((char*)buf.get(), bufSize.x*bufSize.y*bufSize.z);
	system("pause");

	return 0;

}


#endif
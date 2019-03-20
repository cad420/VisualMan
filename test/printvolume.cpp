
#include "../src/volume/volume_utils.h"
#include <iomanip>
#ifndef _DOWNSAMPLE_H_
#define _DOWNSAMPLE_H_


int main(int argc, char* argv[])
{
	std::string fileName;
	std::cin >> fileName;

	LVDReader reader(fileName);
	int blockIndex;

	const auto blockDim = reader.SizeByBlock();
	std::cout << "BlockDim:" << blockDim << std::endl;

	const auto bytes = reader.BlockDataCount();
	std::unique_ptr<unsigned char[]> buf(new unsigned char[bytes]);

	const auto blockSize = reader.BlockSize();
	std::cout << "Block Size:" << blockSize << " " << blockSize << " " << blockSize << std::endl;

	const auto rep = reader.BoundaryRepeat();

	std::vector<int> hist(256);

	int bCount = reader.BlockCount();
	for (int i = 0; i < bCount; i++)
	{
		reader.ReadBlock((char*)buf.get(), i);
		for (int z = 0; z < blockSize; z++)
		{
			for(int y = 0 ;y<blockSize;y++)
			{
				for (int x = 0; x < blockSize; x++)
				{
					if (x >= rep && x < blockSize - rep && y >= rep && y < blockSize - rep && z>=rep && z<blockSize-rep)
					{
						const auto voxelIndex = ysl::Linear(ysl::Point3i(x, y, z), ysl::Size2(blockSize, blockSize));
						hist[buf[voxelIndex]]++;
					}

				}
			}
		}
		std::cout << i << std::endl;
	}

	const auto s = reader.OriginalDataSize();
	const auto totals = s.x*s.y*s.z;
	int i = 0;
	double b = 0;
	for(const auto v:hist)
	{
		b += 1.0*v / totals;
		std::cout << i++ << ": " << v << ", "<<std::fixed<<std::setprecision(2) << b << std::endl;
	}



	//int bx, by, bz;
	//while (std::cin >> bx >> by >> bz)
	//{
	//	const auto blockIndex = ysl::Linear(ysl::Point3i(bx, by, bz), ysl::Size2(blockDim.x, blockDim.y));
	//	std::cout << "BlockIndex:" << blockIndex << std::endl;
	//	reader.ReadBlock((char*)buf.get(), blockIndex);

	//	int z;
	//	while (std::cin >> z)
	//	{
	//		if (z < 0 || z >= blockSize)break;
	//		//	for (int z = 0; z < blockSize; z++)
	//			//{
	//		std::cout << "Slice:" << z << std::endl;
	//		for (int y = blockSize - 1; y >=0; y--)
	//		{
	//			for (int x = 0; x < blockSize; x++)
	//			{
	//				if(x>=rep && x<blockSize-rep && y>=rep && y<blockSize-rep)
	//				{
	//					const auto voxelIndex = ysl::Linear(ysl::Point3i(x, y, z), ysl::Size2(blockSize, blockSize));
	//					std::cout << (int)buf[voxelIndex] << " ";
	//				}

	//			}
	//			std::cout << std::endl;
	//		}
	//		//}
	//	}
	//}
	system("pause");


	return 0;

}


#endif
#include <iostream>
#include <string>
#include "../src/volume/volume_utils.h"
#include "../src/utility/cmdline.h"
#include "../src/volume/volume.h"
#include "abcflowgen.h"
#include <atomic>
#include <thread>
#include "error.h"
#include "timer.h"

#include "../lib/3rdparty/rapidjson/document.h"
#include "objreader.h"


int LVDTester()
{
	std::cout << "LVDTester:\n";

	std::string fileName;
	std::cin >> fileName;

	LVDReader reader(fileName);
	if (reader.valid())
	{
		std::cout << "lvd file information:" << std::endl;
		std::cout << "data size:" << reader.Size();
		std::cout << "size by block:" << reader.SizeByBlock();
		std::cout << "Block size in log:" << reader.BlockSizeInLog() << std::endl;
		std::cout << "Block Size:" << reader.BlockSize() << std::endl;
		std::cout << "Block Data Count:" << reader.BlockDataCount() << std::endl;
		std::cout << "Total Blocks:" << reader.BlockCount() << std::endl;
		std::cout << "Original data size:" << reader.OriginalDataSize() << std::endl;
	}
	else
	{
		std::cout << "Can not open lvd file\n";
		return 0;
	}

	ysl::Log("Enter the block id that you want to write. -1 write all\n");

	int blockId;
	std::cin >> blockId;
	if (blockId == -1)
	{
		const auto s = reader.Size();
		const auto bytes = s.x*s.y*s.z;
		//const auto bytes = 32 * 32 * 32;

		std::unique_ptr<char[]> buffer(new char[bytes]);
		std::cout << " Read lvd file..\n";
		if (reader.BlockSizeInLog() == 5)
		{
			auto data = reader.ReadAll<char, 5>();
			data->GetLinearArray(buffer.get());
			//auto d = data->BlockData(1);
			//::memcpy(buffer.get(), d, bytes);
			std::cout << data->BlockWidth() << std::endl;
			std::cout << data->BlockHeight() << std::endl;
			std::cout << data->BlockDepth() << std::endl;
		}
		else if (reader.BlockSizeInLog() == 6)
		{
			auto data = reader.ReadAll<char, 6>();
			data->GetLinearArray(buffer.get());
		}
		else
		{
			std::cout << "Unsupported block size.\n";
			return 0;
		}
		// Output
		const std::string outFileName{ fileName.substr(0,fileName.find_last_of(".")) + "_fromlvd.raw" };
		std::cout << "Writing as:" << outFileName << std::endl;
		std::ofstream rawFile(outFileName, std::fstream::binary);

		if (!rawFile.is_open())
		{
			std::cout << ".lvd file can not open\n";
			return 0;
		}

		rawFile.write(buffer.get(), bytes);
		std::cout << "Writing finished\n";
	}
	else if (blockId > -1)
	{

		const auto bytes = reader.BlockSize()*reader.BlockSize()*reader.BlockSize();
		std::unique_ptr<char[]> buffer(new char[bytes]);
		std::cout << " Read lvd file..\n";

		do
		{
			if (reader.BlockSizeInLog() == 5)
			{
				auto data = reader.ReadAll<char, 5>();
				memcpy(buffer.get(), data->BlockData(blockId), bytes);
				//data->GetLinearArray(buffer.get());
			}
			else if (reader.BlockSizeInLog() == 6)
			{
				auto data = reader.ReadAll<char, 6>();
				//data->GetLinearArray(buffer.get());
				memcpy(buffer.get(), data->BlockData(blockId), bytes);
			}
			else
			{
				std::cout << "Unsupported block size.\n";
				return 0;
			}
			// Output
			std::stringstream ss;
			ss << blockId;
			std::string str;
			ss >> str;
			const std::string outFileName{ fileName.substr(0,fileName.find_last_of(".")) + "_fromlvd_blockId_" + str + ".raw" };
			std::cout << "Writing as:" << outFileName << std::endl;
			std::ofstream rawFile(outFileName, std::fstream::binary);
			if (!rawFile.is_open())
			{
				std::cout << ".lvd file can not open\n";
				return 0;
			}
			rawFile.write(buffer.get(), bytes);
			std::cout << "Writing finished\n";
		} while (std::cin >> blockId);
	}
	return 0;
}


int jsonTest()
{
	return 0;
}



void CountVolume()
{


	std::string fileName;
	std::cout << "Enter volume file name\n";
	std::cin >> fileName;

	int leftTh, rightTh;
	std::cout << "Enter min and max threshold value\n";
	std::cin >> leftTh >> rightTh;


	std::size_t blockSize;
	std::cout << "Enter block size (e.g. 32,64,128,256,512,..)\n";
	std::cin >> blockSize;

	std::size_t w, h, d, v;
	std::cout << "Enter (x,y,z,v)\n";
	std::cin >> w >> h >> d >> v;

	double ratio;

	std::cout << "Enter ratio:\n";
	std::cin >> ratio;

	const auto bytes = w * h*d*v;

	std::unique_ptr<unsigned char[]> buf(new unsigned char[bytes]);

	RawReader reader(fileName, { w,h,d }, v);

	reader.readRegion({ 0,0,0 }, { w,h,d }, buf.get());


	std::cout << "Reading finished\n";

	std::size_t totalBlocks = 0, validBlock = 0;

#pragma parallel for

	for(int z = 0 ; z<d;z+=blockSize)
	{
		for(int y = 0 ;y<h;y+=blockSize)
		{
			for(int x = 0;x<w;x+=blockSize)
			{
				std::size_t validVoxels = 0;
				for (int zz = 0; zz < blockSize; zz++)
				{
					for (int yy = 0; yy < blockSize; yy++)
					{
						for (int xx = 0; xx < blockSize; xx++)
						{
							const auto X = x + xx, Y = y + yy, Z = z + zz;
							const auto linear = ysl::Linear({ x,y,z }, { w,h });
							if (buf[linear] >= leftTh && buf[linear] <= rightTh)
							{
								validVoxels++;
							}
						}
					}
				}
				if (validVoxels >= ratio * blockSize*blockSize*blockSize)
					validBlock++;
				totalBlocks++;

			}
		}
	}
	std::cout << validBlock << " of " << totalBlocks << ":" << 1.0*validBlock / totalBlocks << std::endl;
}



int main(int argc, char *argv[])
{


	int x, y, z, xc, yc, zc;
	//std::cin >> x >> y >> z;
	//ABCFlowGen(x,y,z);
	//SimpleBlockGen(x,y,z,2,2,2);
	std::string fileName;
	std::cin >> fileName;
	//int x, y, z, repeat;
	int repeat;
	std::cin >> x >> y >> z >> repeat;
	RawToLVDConverter<7> converter(fileName,x,y,z,repeat);
	converter.convert();
	converter.save(fileName);
	//LVDTester();
	//ysl::ObjReader reader;
	//reader.Load("C:\\Users\\ysl\\Desktop\\dragon.obj");

	//std::cout << reader.getFaceIndices().size() << std::endl;
	//system("pause");
	//CountVolume();
	system("pause");
	return 0;

}


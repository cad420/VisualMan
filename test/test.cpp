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

int main(int argc, char *argv[])
{
	//int x, y, z, xc, yc, zc;
	//std::cin >> x >> y >> z;
	//ABCFlowGen(x,y,z);
	////SimpleBlockGen(x,y,z,2,2,2);
	//std::string fileName;
	//std::cin >> fileName;
	////int x, y, z, repeat;
	//int repeat;
	//std::cin >> x >> y >> z >> repeat;
	//RawToLVDConverter<6> converter(fileName,x,y,z,repeat);
	//converter.convert();
	//converter.save(fileName);
	//LVDTester();

	system("pause");
	return 0;

}


#include <iostream>
#include <string>

#include "../src/volume/volume_utils.h"

#include "../src/utility/cmdline.h"
#include "../src/volume/volume.h"
#include "../src/volume/virtualvolumehierachy.h"
#include <forward_list>
#include "abcflowgen.h"

#include <atomic>
#include <thread>


int LVDTester()
{
	std::cout << "LVDTester:\n";

	std::string fileName;

	std::cin >> fileName;

	BlockVolumeReader reader(fileName);
	if (reader.valid())
	{
		std::cout << "lvd file information:" << std::endl;
		std::cout << "X:" << reader.width() << std::endl;
		std::cout << "Y:" << reader.height() << std::endl;
		std::cout << "Z:" << reader.depth() << std::endl;
		std::cout << "xBlock:" << reader.xBlockCount() << std::endl;
		std::cout << "yBlock:" << reader.yBlockCount() << std::endl;
		std::cout << "zBlock:" << reader.zBlockCount() << std::endl;
		std::cout << "Block size in log:" << reader.blockSizeInLog() << std::endl;
		std::cout << "Block Size:" << reader.blockSize() << std::endl;
		std::cout << "Block Data Count:" << reader.blockDataCount() << std::endl;
		std::cout << "Total Blocks:" << reader.totalBlocks() << std::endl;
	}
	else
	{
		std::cout << "Can not open lvd file\n";
		return 0;
	}
	const auto bytes = reader.width()*reader.height()*reader.depth();
	//const auto bytes = 32 * 32 * 32;
	std::unique_ptr<char[]> buffer(new char[bytes]);
	std::cout << " Read lvd file..\n";
	if (reader.blockSizeInLog() == 5)
	{
		auto data = reader.readAll<char, 5>();
		data->GetLinearArray(buffer.get());
		//auto d = data->BlockData(1);
		//::memcpy(buffer.get(), d, bytes);
		std::cout << data->BlockWidth() << std::endl;
		std::cout << data->BlockHeight() << std::endl;
		std::cout << data->BlockDepth() << std::endl;
	}
	else if (reader.blockSizeInLog() == 6)
	{
		auto data = reader.readAll<char, 6>();
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
	return 0;

}

int g_sum = 0;

std::atomic_bool flag;

int atomicTest()
{
	auto f = []()
	{
		for(int i = 1 ;i<=10000000;i++)
		{
			bool expected = false;
			while (flag.compare_exchange_strong(expected, true) != true)
				expected = false;
			g_sum += 1;
			flag = false;
			//std::cout << i << std::endl;
		}
	};
	std::thread t1(f);
	std::thread t2(f);
	t1.join();
	t2.join();
	std::cout << g_sum << std::endl;
	return 0;
}

int main(int argc, char *argv[])
{
	int x, y, z;
	std::cin >> x >> y >> z;
	ABCFlowGen(x,y,z);
	std::string fileName;
	std::cin >> fileName;
	//int x, y, z, repeat;
	int repeat;
	std::cin >> x >> y >> z >> repeat;
	RawToLVDConverter<6> converter(fileName,x,y,z,repeat);
	converter.convert();
	converter.save(fileName);


	LVDTester();

	system("pause");
	return 0;

}


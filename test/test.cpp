#include <iostream>
#include <string>

#include "../src/volume/volume_utils.h"

#include "../src/utility/cmdline.h"




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

int main(int argc, char *argv[])
{
	//std::string fileName;
	//int x, y, z, repeat;
	//std::cin >> fileName >> x >> y >> z >> repeat;
	//RawToLVDConverter<5> lvdConverter(fileName, x, y, z, repeat);
	//lvdConverter.setBoundaryValue(0);
	//lvdConverter.convert();
	//lvdConverter.save(fileName);
	//LVDTester();
	 // create a parser

	//std::string fileName;
	//std::cin >> fileName;

	//ysl::TransferFunction tf(fileName);
	//if(!tf.valid())
	//{
	//	std::cout << "faild\n";
	//	return 0;
	//}

	//std::vector<ysl::RGBASpectrum> data{256};
	//tf.FetchData(data.data(), 256);

	//for(int i = 0 ; i < 256;i++)
	//{
	//	std::cout << data[i] << std::endl;
	//}


	char str[5][85];
	for (int i = 0; i < 5; i++)
		scanf("%s", str[i]);

	for (int i = 4; i > 0; i--) {
		for (int j = 0; j < i; j++) {
			if (strcmp(str[j], str[j + 1]) > 0) {
				char t[85];
				strcpy(t, str[j]);
				strcpy(str[j], str[j + 1]);
				strcpy(str[j + 1], t);
			}
		}
	}
	printf("After sorted:\n");
	for (int i = 0; i < 5; i++)
		printf("%s\n", str[i]);


	system("pause");
	return 0;

}


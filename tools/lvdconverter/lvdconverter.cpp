#include <iostream>
#include <string>
#include "../../src/io/lvdconverter.h"

//
//int LVDTester()
//{
//	std::cout << "LVDTester:\n";
//
//	std::string fileName;
//	std::cin >> fileName;
//
//	ysl::LVDReader reader(fileName);
//	if (reader.Valid())
//	{
//		std::cout << "lvd file information:" << std::endl;
//		std::cout << "data size:" << reader.Size();
//		std::cout << "size by block:" << reader.SizeByBlock();
//		std::cout << "Block size in log:" << reader.BlockSizeInLog() << std::endl;
//		std::cout << "Block Size:" << reader.BlockSize() << std::endl;
//		std::cout << "Block Data Count:" << reader.BlockDataCount() << std::endl;
//		std::cout << "Total Blocks:" << reader.BlockCount() << std::endl;
//		std::cout << "Original data size:" << reader.OriginalDataSize() << std::endl;
//	}
//	else
//	{
//		std::cout << "Can not open lvd file\n";
//		return 0;
//	}
//
//	ysl::Log("Enter the block id that you want to write. -1 write all\n");
//
//	int blockId;
//	std::cin >> blockId;
//	if (blockId == -1)
//	{
//		const auto s = reader.Size();
//		const auto bytes = s.x*s.y*s.z;
//		//const auto bytes = 32 * 32 * 32;
//
//		std::unique_ptr<char[]> buffer(new char[bytes]);
//		std::cout << " Read lvd file..\n";
//		if (reader.BlockSizeInLog() == 5)
//		{
//			auto data = reader.ReadAll<char, 5>();
//			data->GetLinearArray(buffer.get());
//			//auto d = data->BlockData(1);
//			//::memcpy(buffer.get(), d, bytes);
//			std::cout << data->BlockWidth() << std::endl;
//			std::cout << data->BlockHeight() << std::endl;
//			std::cout << data->BlockDepth() << std::endl;
//		}
//		else if (reader.BlockSizeInLog() == 6)
//		{
//			auto data = reader.ReadAll<char, 6>();
//			data->GetLinearArray(buffer.get());
//		}
//		else
//		{
//			std::cout << "Unsupported block size.\n";
//			return 0;
//		}
//		// Output
//		const std::string outFileName{ fileName.substr(0,fileName.find_last_of(".")) + "_fromlvd.raw" };
//		std::cout << "Writing as:" << outFileName << std::endl;
//		std::ofstream rawFile(outFileName, std::fstream::binary);
//
//		if (!rawFile.is_open())
//		{
//			std::cout << ".lvd file can not open\n";
//			return 0;
//		}
//
//		rawFile.write(buffer.get(), bytes);
//		std::cout << "Writing finished\n";
//	}
//	else if (blockId > -1)
//	{
//
//		const auto bytes = reader.BlockSize()*reader.BlockSize()*reader.BlockSize();
//		std::unique_ptr<char[]> buffer(new char[bytes]);
//		std::cout << " Read lvd file..\n";
//
//		do
//		{
//			if (reader.BlockSizeInLog() == 5)
//			{
//				auto data = reader.ReadAll<char, 5>();
//				memcpy(buffer.get(), data->BlockData(blockId), bytes);
//				//data->GetLinearArray(buffer.get());
//			}
//			else if (reader.BlockSizeInLog() == 6)
//			{
//				auto data = reader.ReadAll<char, 6>();
//				//data->GetLinearArray(buffer.get());
//				memcpy(buffer.get(), data->BlockData(blockId), bytes);
//			}
//			else
//			{
//				std::cout << "Unsupported block size.\n";
//				return 0;
//			}
//			// Output
//			std::stringstream ss;
//			ss << blockId;
//			std::string str;
//			ss >> str;
//			const std::string outFileName{ fileName.substr(0,fileName.find_last_of(".")) + "_fromlvd_blockId_" + str + ".raw" };
//			std::cout << "Writing as:" << outFileName << std::endl;
//			std::ofstream rawFile(outFileName, std::fstream::binary);
//			if (!rawFile.is_open())
//			{
//				std::cout << ".lvd file can not open\n";
//				return 0;
//			}
//			rawFile.write(buffer.get(), bytes);
//			std::cout << "Writing finished\n";
//		} while (std::cin >> blockId);
//	}
//	return 0;
//}

//
//int jsonTest()
//{
//	return 0;
//}

//
//
//void CountVolume()
//{
//
//
//	std::string fileName;
//	std::cout << "Enter volume file name\n";
//	std::cin >> fileName;
//
//	int leftTh, rightTh;
//	std::cout << "Enter min and max threshold value\n";
//	std::cin >> leftTh >> rightTh;
//
//
//	std::size_t blockSize;
//	std::cout << "Enter block size (e.g. 32,64,128,256,512,..)\n";
//	std::cin >> blockSize;
//
//	std::size_t w, h, d, v;
//	std::cout << "Enter (x,y,z,v)\n";
//	std::cin >> w >> h >> d >> v;
//
//	double ratio;
//
//	std::cout << "Enter ratio:\n";
//	std::cin >> ratio;
//
//	const auto bytes = w * h*d*v;
//
//	std::unique_ptr<unsigned char[]> buf(new unsigned char[bytes]);
//
//	ysl::RawReader reader(fileName, { w,h,d }, v);
//
//	reader.readRegion({ 0,0,0 }, { w,h,d }, buf.get());
//
//
//	std::cout << "Reading finished\n";
//
//	std::size_t totalBlocks = 0, validBlock = 0;
//
//#pragma parallel for
//
//	for(int z = 0 ; z<d;z+=blockSize)
//	{
//		for(int y = 0 ;y<h;y+=blockSize)
//		{
//			for(int x = 0;x<w;x+=blockSize)
//			{
//				std::size_t validVoxels = 0;
//				for (int zz = 0; zz < blockSize; zz++)
//				{
//					for (int yy = 0; yy < blockSize; yy++)
//					{
//						for (int xx = 0; xx < blockSize; xx++)
//						{
//							const auto X = x + xx, Y = y + yy, Z = z + zz;
//							const auto linear = ysl::Linear({ x,y,z }, { w,h });
//							if (buf[linear] >= leftTh && buf[linear] <= rightTh)
//							{
//								validVoxels++;
//							}
//						}
//					}
//				}
//				if (validVoxels >= ratio * blockSize*blockSize*blockSize)
//					validBlock++;
//				totalBlocks++;
//
//			}
//		}
//	}
//	std::cout << validBlock << " of " << totalBlocks << ":" << 1.0*validBlock / totalBlocks << std::endl;
//}



int main(int argc, char *argv[])
{
	std::cout << "[filename(str), offset(std::size_t), x(int) ,y(int) ,z(int), padding(int),log(int), outfilename(str)]\n";
	int x, y, z, log;

	//std::cin >> x >> y >> z;
	//ABCFlowGen(x,y,z);
	//SimpleBlockGen(x,y,z,2,2,2);

	std::string fileName;
	std::cin >> fileName;

	std::size_t offset;
	std::cin >> offset;

	int repeat;
	std::cin >> x >> y >> z >> repeat >> log;
	std::string outFileName;
	std::cin >> outFileName;

	if(log == 6)
	{
		ysl::RawToLVDConverter<6> converter(fileName, x, y, z, repeat, outFileName,offset);
		converter.convert();
		converter.save(fileName);
	}else if(log == 7)
	{
		ysl::RawToLVDConverter<7> converter(fileName, x, y, z, repeat, outFileName,offset);
		converter.convert();
		converter.save(fileName);
	}else
	{
		std::cout << "Unsupported block size\n";
		return 0;
	}

	system("pause");
	return 0;
}


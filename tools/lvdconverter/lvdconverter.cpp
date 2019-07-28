#include <iostream>
#include <string>
#include <lvdconverter.h>

void modifyLVDHeader()
{
	std::string fileName;
	std::cout << "[filename(str)]\n";
	std::cin >> fileName;

	std::fstream file(fileName, std::ios_base::in | std::ios_base::out);

	if(!file.is_open())
	{
		throw std::runtime_error("Can not load .lvd file\n");
	}
	ysl::LVDHeader header;

	unsigned char buf[LVD_HEADER_SIZE];
	file.seekg(0, std::ios::beg);
	file.read((char*)buf, LVD_HEADER_SIZE);
	header.Decode(buf);

	std::cout << "Valid Data Size:" << header.originalDataDim[0]<<" "<< header.originalDataDim[1]<<" "<< header.originalDataDim[2] << std::endl;

	ysl::Vec3i size;
	std::cout << "Modify to:" << std::endl;
	std::cin >> size.x >> size.y >> size.z;
	header.originalDataDim[0] = size.x;
	header.originalDataDim[1] = size.x;
	header.originalDataDim[2] = size.x;

	file.seekg(0, std::ios::beg);
	file.write((char*)header.Encode(), LVD_HEADER_SIZE);
	return;
}

void convert()
{
	std::cout << "[filename(str), offset(std::size_t), x(int) ,y(int) ,z(int), padding(int) ,log(int), outfilename(str)]\n";
	int x, y, z, log;
	std::string fileName;
	std::cin >> fileName;

	std::size_t offset;
	std::cin >> offset;

	int repeat;
	std::cin >> x >> y >> z >> repeat >> log;
	std::string outFileName;
	std::cin >> outFileName;

	if (log == 5)
	{
		ysl::RawToLVDConverterEx<5> converter(fileName, x, y, z, repeat, outFileName, offset);
		//ysl::RawToLVDConverter<6> converter(fileName, x, y, z, repeat, outFileName,offset);
		converter.convert();
		converter.save(fileName);
	}
	else if (log == 6)
	{
		ysl::RawToLVDConverterEx<6> converter(fileName, x, y, z, repeat, outFileName, offset);
		//ysl::RawToLVDConverter<6> converter(fileName, x, y, z, repeat, outFileName,offset);
		converter.convert();
		converter.save(fileName);
	}
	else if (log == 7)
	{
		ysl::RawToLVDConverterEx<7> converter(fileName, x, y, z, repeat, outFileName, offset);
		//ysl::RawToLVDConverter<7> converter(fileName, x, y, z, repeat, outFileName,offset);
		converter.convert();
		converter.save(fileName);
	}
	else
	{
		std::cout << "Unsupported block size\n";
	}
}

int main(int argc, char *argv[])
{

	char op;
	std::cout << "Enter options:" << std::endl;
	std::cout << "c to convert .raw to .lvd\n";
	std::cout << "m to modified header\n";
	std::cin >> op;
	if(op == 'm')
	{
		modifyLVDHeader();
	}else if(op == 'c')
	{
		convert();
	}
	return 0;
}






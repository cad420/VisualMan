#include <iostream>
#include <string>
#include <lvdconverter.h>


int main(int argc, char *argv[])
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
	else if(log == 6)
	{
		ysl::RawToLVDConverterEx<6> converter(fileName, x, y, z, repeat, outFileName,offset);
		//ysl::RawToLVDConverter<6> converter(fileName, x, y, z, repeat, outFileName,offset);
		converter.convert();
		converter.save(fileName);
	}else if(log == 7)
	{
		ysl::RawToLVDConverterEx<7> converter(fileName, x, y, z, repeat, outFileName,offset);
		//ysl::RawToLVDConverter<7> converter(fileName, x, y, z, repeat, outFileName,offset);
		converter.convert();
		converter.save(fileName);
	}else
	{
		std::cout << "Unsupported block size\n";
		return 0;
	}


	return 0;
}






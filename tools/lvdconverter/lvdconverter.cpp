#include <iostream>
#include <string>
#include <lvdconverter.h>
#include <cmdline.h>


void convert()
{

}

int main(int argc, char *argv[])
{

	cmdline::parser a;
	a.add<std::string>("if", 'i', ".raw input filename", true);
	a.add<std::size_t>("offset", 'o', "offset converted from", true);
	a.add<int>("width", 'w', "width of the raw file", true);
	a.add<int>("height", 'h', "height of the raw file", true);
	a.add<int>("depth", 'd', "depth of the raw file", true);
	a.add<int>("padding",'p',"padding of the block, just support for 0, 1 or 2. the default value is 2", false, 2,cmdline::oneof<int>(0,1,2));
	a.add<int>("side", 's', "the side length of the block, which is represented in logarithm, only support 5 ,6 and 7. The Default value is 6", false ,6,cmdline::oneof<int>(5,6,7));
	a.add<std::string>("of", 'f', ".lvd output filename", true);


	//std::cout<<a.usage();
	a.parse_check(argc, argv);

	//std::cout << "[filename(str), offset(std::size_t), x(int) ,y(int) ,z(int), padding(int) ,log(int), outfilename(str)]\n";


	auto fileName = a.get<std::string>("if");
	auto x = a.get<int>("width");
	auto y = a.get<int>("height");
	auto z = a.get<int>("depth");
	auto offset = a.get<std::size_t>("offset");
	auto repeat= a.get<int>("padding");
	auto outFileName = a.get<std::string>("of");
	auto log = a.get<int>("side");


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






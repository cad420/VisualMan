#include <iostream>
#include <string>
#include <lvdconverter.h>
#include <cmdline.h>

int main(int argc, char *argv[])
{

	cmdline::parser a;
	a.add<std::string>("if", 'i', ".raw input filename", true);
	a.add<std::size_t>("offset", 'o', "offset converted from", true);
	a.add<int>("width", 'w', "width of the raw file", true);
	a.add<int>("height", 'h', "height of the raw file", true);
	a.add<int>("depth", 'd', "depth of the raw file", true);
	a.add<int>("memlimit", 'm', "maximum memory limit in Gb", false, 128);
	a.add<int>("padding",'p',"padding of the block, just support for 0, 1 or 2. the default value is 2", false, 2,cmdline::oneof<int>(0,1,2));
	a.add<int>("side", 's', "the side length of the block, which is represented in logarithm. The Default value is 6.", false, 6, cmdline::oneof<int>(5,6,7,8,9,10,11,12,13,14));
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
	
#define IMPL_RAW_TO_LVD_LOG(N)											\
	case N: {															\
		ysl::RawToLVDConverterEx<N>										\
			converter(fileName, x, y, z, repeat, outFileName, offset);	\
		converter.convert();											\
		converter.save();												\
	} break

	switch (log) {
	IMPL_RAW_TO_LVD_LOG(5);
	IMPL_RAW_TO_LVD_LOG(6);
	IMPL_RAW_TO_LVD_LOG(7);
	IMPL_RAW_TO_LVD_LOG(8);
	IMPL_RAW_TO_LVD_LOG(9);
	IMPL_RAW_TO_LVD_LOG(10);
	IMPL_RAW_TO_LVD_LOG(11);
	IMPL_RAW_TO_LVD_LOG(12);
	IMPL_RAW_TO_LVD_LOG(13);
	IMPL_RAW_TO_LVD_LOG(14);
	default: std::cout << "Unsupported block size\n";
	}
}


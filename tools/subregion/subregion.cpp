
#include <iostream>
#include <rawreader.h>
int main()
{
	size_t w, h, d, x, y, z, w0, h0, d0, vs;
	std::string inFileName;
	std::string outFileName;
	std::cout << "[w, h, d, x,y,z,w0,h0,d0,voxelSize, inFileName(str), outFileName(str)]\n";
	std::cin >> w >> h >> d >> x >> y >> z >> w0 >> h0 >> d0 >> vs>>inFileName >> outFileName;

	auto size = ysl::Size3(w, h, d);
	auto subsize = ysl::Size3(w0, h0, d0);
	auto begin = ysl::Size3(x, y, z);
	ysl::RawReader raw(inFileName, size, vs);

	std::cout << "Read From " << begin << ". Size is " << subsize << std::endl;

	auto io = ysl::Object::CreateObject<ysl::IPluginFileMap>("common.filemapio");
	if (io == nullptr)
	{
		std::cout << "Can not load io plugin\n";
		return 1;
	}

	const auto bytes = w0 * h0*d0*vs;
	if(io->Open(outFileName, bytes, ysl::FileAccess::ReadWrite , ysl::MapAccess::ReadWrite) == false)
	{
		std::cout << "Create File Map Object Failed\n";
		return 2;
	}

	auto ptr = io->FileMemPointer(0, bytes);
	if(ptr == nullptr)
	{
		std::cout << "Map pointer is nullptr\n";
		return 3;
	}
	auto readBytes = raw.readRegion(begin, subsize, ptr);
	if(readBytes != bytes)
	{
		std::cout << " Read byte do not match expected bytes\n";
		return 4;
	}
	io->Close();
	return 0;
}
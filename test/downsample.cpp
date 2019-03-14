
#ifndef _DOWNSAMPLE_H_
#define _DOWNSAMPLE_H_

#include "../src/volume/volume_utils.h"

int main(int argc, char* argv[])
{
	std::string fileName;
	std::size_t x, y, z, s;
	std::cin >> fileName >> x >> y >> z >> s;

	RawReader reader(fileName,ysl::Size3{x,y,z},s);
	std::unique_ptr<unsigned char> buf(new unsigned char[x*y*z*s]);
	reader.readRegion({ 0,0,0 }, { x,y,z }, buf.get());

	return 0;
}


#endif
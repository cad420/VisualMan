
#include "rawio.h"

namespace ysl
{
	AbstraFileMap::AbstraFileMap(const std::string& fileName, std::size_t fileSize,int flags) :fileName(fileName),fileSize(fileSize),flags(flags)
	{

	}

	AbstraFileMap::~AbstraFileMap()
	{
	}
}

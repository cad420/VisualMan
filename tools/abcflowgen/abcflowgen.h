#ifndef _ABCFLOWGEN_H_
#define _ABCFLOWGEN_H_


#include <cstddef>

class ABCFlowGenerator
{
public:
	static int ABCFlowGen(std::size_t x, std::size_t y, std::size_t z);
	static int SimpleBlockGen(std::size_t x, std::size_t y, std::size_t z, int xColor, int yColor, int zColor);
};




#endif /*_ABCFLOWGEN_H_*/
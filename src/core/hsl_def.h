#ifndef __HSL_DEF_H_
#define __HSL_DEF_H_

#include <limits>

namespace hsl 
{
	typedef float Float;

	constexpr Float PI = 3.1415926535;

	constexpr Float LOWEST_Float_VALUE = std::numeric_limits<Float>::lowest();

	constexpr Float MAX_Float_VALUE = (std::numeric_limits<Float>::max)();		// For fucking min/max defined in windows.h
}


#endif /*__HSL_DEF_H_*/

#ifndef SAMPLER_H_
#define SAMPLER_H_
//#include "core.h"
#include <cmath>

#include "basetype.h"

/*
*
*/
namespace ysl {


	constexpr Float Pi = 3.14159265358979323846;

	constexpr Float LOWEST_Float_VALUE = (std::numeric_limits<Float>::lowest)();

	constexpr Float MAX_Float_VALUE = (std::numeric_limits<Float>::max)();		// For fucking min/max macro defined in windows.h


	template<typename T, typename U, typename V> inline
	T
	Clamp(const T & value, const U & low, const V & high)
	{
		if (value < low)return low;
		if (value > high)return high;
		return value;
	}

	/*
	* This modulus function always guarantees a positive result
	* and (a/b)*b + a%b == a always holds
	*/

	template<typename T> inline
	T
	Mod(const T & a, const T & b)
	{
		const auto r = a - (a / b)*b;
		return static_cast<T>(r > 0 ? r : r + b);
	}

	template<> inline
	Float
	Mod<Float>(const Float& a, const Float& b)
	{
		return std::fmod(static_cast<double>(a), static_cast<double>(b));
	}


	template<typename T> inline
	bool
	IsNaN(const T & t)
	{
		return std::isnan(t);
	}

	template<> inline
	bool
	IsNaN(const int & t)
	{
		return false;
	}



	inline
	Float
	Lerp(Float t, Float v1, Float v2)
	{
		return (1 - t)*v1 + t * v2;
	}

	inline
	Float
	DegreesToRadians(Float degrees)
	{
		return degrees * Float(Pi / 180);
	}

	inline
	Float
	RadiansToDegrees(Float radians)
	{
		return radians * Float(180 / Pi);
	}
	

}

#endif

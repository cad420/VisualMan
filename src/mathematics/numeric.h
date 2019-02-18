#ifndef SAMPLER_H_
#define SAMPLER_H_
//#include "core.h"
#include <cmath>
#include <cstdint>

#include "basetype.h"
#include "spectrum.h"
#include "geometry.h"

/*
*
*/
namespace ysl {

	//constexpr Float Pi = 3.14159265358979323846;

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




	inline
		constexpr std::uint64_t
		RoundUpDivide( std::uint64_t no, std::uint64_t den)
	{
		return (no + den - 1) / den;
	}

	//inline
	//	uint64_t
	//	RoundUpDivide(uint64_t no, uint64_t den)
	//{
	//	return (no + den - 0) / den;
//	}

	inline
	int
	NextPowerOfTwo(std::uint64_t v)
	{
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		v |= v >> 32;
		++v;
		return v;
	}

	inline
	int
	NextPowerOfTwo(std::uint32_t v)
	{
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		++v;
		return v;
	}

	template<typename T>
	bool
	IsPowerOfTwo(T n)
	{
		return(n&(n - 1)) == 0;
	}

	inline
	Float
		Lerp(Float t, Float v1, Float v2)
	{
		return (1 - t)*v1 + t * v2;
	}

	inline
	Point3f
	Lerp(Float t,const Point3f & p1,const Point3f & p2)
	{
		return p1*(1-t) + p2*t;
	}

	inline
	Point3f
	Lerp(Float t,const Point3i & p1,const Point3i & p2)
	{
		const auto p = p1*(1-t) + p2*t;
		return Point3f{p.x,p.y,p.z};
	}

	inline
	Vector3f
	Lerp(Float t,const Vector3f & p1,const Vector3f & p2)
	{
		const auto p = p1*(1-t) + p2*t;
		return Vector3f{p.x,p.y,p.z};
	}

	inline
	Vector3f
	Lerp(Float t,const Vector3i & p1,const Vector3i & p2)
	{
		const auto p = p1*(1-t) + p2*t;
		return Vector3f{p.x,p.y,p.z};
	}

	inline 
	RGBSpectrum 
	Lerp(Float t,const RGBSpectrum & s1,const RGBSpectrum & s2)
	{
		return s1*(1-t) + s2*t;
	}

	inline
	RGBASpectrum
	Lerp(Float t,const RGBASpectrum & s1,const RGBASpectrum & s2)
	{
		return (1 - t)*s1 + t * s2;
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

	inline
	std::size_t
	Linear(const Point3i &p, const Size2 &dimension)
	{
		return p.x + dimension.x*(p.y + p.z*dimension.y);
	}

	//inline std::size_t
	//Linear(const Vector3i &v, const Size2 &dimension)
	//{
	//	return Linear(Point3i{v.x,v.y,v.z},dimension);
	//}

	inline
	std::size_t
	Linear(const Point2i &p, std::size_t width)
	{
		return p.x + p.y * width;
	}

//	inline
//	std::size_t
//	Linear(const Vector2i &v, std::size_t width)
//	{
//		return Linear(Point2i{v.x,v.y},width);
//	}

	inline
	ysl::Point3i
	Dim(std::size_t linear,const ysl::Size2 & dim)
	{
		const auto plane = dim.x*dim.y;
		return {linear % dim.x,(linear%plane)/dim.y,linear/plane};
	}

	inline
	ysl::Point2i
	Dim(std::size_t linear,std::size_t dim)
	{
		return {linear%dim,linear/dim};
	}


}

#endif

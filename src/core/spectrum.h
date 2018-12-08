#ifndef SPECTRUM_H_
#define SPECTRUM_H_
#include <cassert>
#include "../mathematics/basetype.h"


namespace ysl 
{
	template<int nSamples>
	class CoefficientSpectrum 
	{
	public:
		Float c[nSamples];

		explicit CoefficientSpectrum(Float v = 0.0) 
		{
			for(auto i = 0 ; i < nSamples ; i++) 
				c[i] = v;
		}

		//CoefficientSpectrum(const CoefficientSpectrum & s) 
		//{
		//	for(auto i = 0 ;i<nSamples;i++)
		//		c[i] = s.c[i];
		//}

		//CoefficientSpectrum& operator=(const CoefficientSpectrum & s) 
		//{
		//	for(auto i = 0;i<nSamples;i++) {
		//		c[i] = s.c[i];
		//	}
		//	return *this;
		//}

		bool HasNaNs()const 
		{
			for(auto i = 0 ; i < nSamples ; i++) 
				if (std::isnan(c[i]))
					return true;
			return false;
		}

		CoefficientSpectrum& operator+=(const CoefficientSpectrum & spectrum) 
		{
			for(auto i = 0 ;i<nSamples;i++)
				c[i] += spectrum.c[i];
			return *this;
		}

		CoefficientSpectrum& operator-=(const CoefficientSpectrum & spectrum) {
			for (auto i = 0; i < nSamples; i++)
				c[i] -= spectrum.c[i];
			return *this;
		}

		CoefficientSpectrum& operator*=(const CoefficientSpectrum & spectrum) {
			for (auto i = 0; i < nSamples; i++)
				c[i] *= spectrum.c[i];
			return *this;
		}

		CoefficientSpectrum& operator/=(const CoefficientSpectrum & spectrum) {

			assert(!spectrum.HasNaNs());
			for (auto i = 0; i < nSamples; i++)
				c[i] /= spectrum.c[i];
			return *this;
		}

		bool IsBlack()const {
			for (auto i = 0; i < nSamples; i++)
				if (c[i] != 0.f)
					return false;
			return true;
		}

		CoefficientSpectrum Clamp(Float low,Float high)const
		{

			//Spectrum spectrum;
			//for(auto i = 0 ; i < nSamples;i++) {
			//	spectrum.c[i] = hsl::Clamp(low, high);
			//}
			assert(false);
			return CoefficientSpectrum{};
		}

		void Clamped(Float low, Float high) {
			//for (auto i = 0; i < nSamples; i++) {
			//	c[i] = hsl::Clamp(low, high);
			//}
			//return spectrum;

			assert(false);
		}

	};

	template<int nSamples>
	CoefficientSpectrum<nSamples> operator+(const CoefficientSpectrum<nSamples> & s1, const CoefficientSpectrum<nSamples> & s2)
	{
		return CoefficientSpectrum<nSamples>(s1) += s2;			// More Effective C++: Item 22
	}

	template<int nSamples>
	CoefficientSpectrum<nSamples> operator-(const CoefficientSpectrum<nSamples> & s1, const CoefficientSpectrum<nSamples> & s2)
	{
		return CoefficientSpectrum<nSamples>(s1) -= s2;			// More Effective C++: Item 22
	}
	template<int nSamples>
	CoefficientSpectrum<nSamples> operator*(const CoefficientSpectrum<nSamples> & s1, const CoefficientSpectrum<nSamples> & s2)
	{
		return CoefficientSpectrum<nSamples>(s1) *= s2;			// More Effective C++: Item 22
	}
	template<int nSamples>
	CoefficientSpectrum<nSamples> operator/(const CoefficientSpectrum<nSamples> & s1, const CoefficientSpectrum<nSamples> & s2)
	{
		return CoefficientSpectrum<nSamples>(s1) /= s2;			// More Effective C++: Item 22
	}


	class RGBSpectrum: public CoefficientSpectrum<3>
	{
	public:
		explicit RGBSpectrum(Float v = 0.f):CoefficientSpectrum<3>(v){}
		explicit RGBSpectrum(const Float * rgb) 
		{
			c[0] = rgb[0];
			c[1] = rgb[1];
			c[2] = rgb[2];
		}
		void ToRGB(Float * rgb) 
		{
			rgb[0] = c[0];
			rgb[1] = c[1];
			rgb[2] = c[2];
		}
	};

	using Spectrum = RGBSpectrum;
}

#endif

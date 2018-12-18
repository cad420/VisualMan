#ifndef SPECTRUM_H_
#define SPECTRUM_H_
#include <cassert>
#include <fstream>
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

		explicit  CoefficientSpectrum(Float * v)
		{
			for (auto i = 0; i < nSamples; i++)
				c[i] = v[i];
		}

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

		CoefficientSpectrum& operator*=(Float s) {
			for (auto i = 0; i < nSamples; i++)
				c[i] *= s;
			return *this;
		}

		CoefficientSpectrum& operator/=(const CoefficientSpectrum & spectrum) {

			assert(!spectrum.HasNaNs());
			for (auto i = 0; i < nSamples; i++)
				c[i] /= spectrum.c[i];
			return *this;
		}

		CoefficientSpectrum& operator/=(Float s) {

			assert(!spectrum.HasNaNs());
			Float inv = 1 / s;
			for (auto i = 0; i < nSamples; i++)
				c[i] *= inv;
			return *this;
		}

		bool IsBlack()const {
			for (auto i = 0; i < nSamples; i++)
				if (c[i] != 0.f)
					return false;
			return true;
		}

		void ToArray(Float * arr)const
		{
			for (auto i = 0; i < nSamples; i++)
				*(arr + i) = c[i];

		}

	};

	template<int nSamples>
	CoefficientSpectrum<nSamples> 
	operator+(const CoefficientSpectrum<nSamples> & s1, const CoefficientSpectrum<nSamples> & s2)
	{
		return CoefficientSpectrum<nSamples>(s1) += s2;			// More Effective C++: Item 22
	}

	template<int nSamples>
	CoefficientSpectrum<nSamples> 
	operator-(const CoefficientSpectrum<nSamples> & s1, const CoefficientSpectrum<nSamples> & s2)
	{
		return CoefficientSpectrum<nSamples>(s1) -= s2;			// More Effective C++: Item 22
	}
	template<int nSamples>
	CoefficientSpectrum<nSamples> 
	operator*(const CoefficientSpectrum<nSamples> & s1, const CoefficientSpectrum<nSamples> & s2)
	{
		return CoefficientSpectrum<nSamples>(s1) *= s2;			// More Effective C++: Item 22
	}

	template<int nSamples>
	CoefficientSpectrum<nSamples> 
	operator/(const CoefficientSpectrum<nSamples> & s1, const CoefficientSpectrum<nSamples> & s2)
	{
		return CoefficientSpectrum<nSamples>(s1) /= s2;			// More Effective C++: Item 22
	}

	template<int nSamples>
	CoefficientSpectrum<nSamples> 
	operator*(const CoefficientSpectrum<nSamples> & s,Float v)
	{
		auto r = s;
		for (auto i = 0; i < nSamples; i++) r.c[i] *= v;
		return r;
	}

	template<int nSamples>
	CoefficientSpectrum<nSamples> 
	operator*(Float v, const CoefficientSpectrum<nSamples> & s)
	{
		return s * v;
	}

	template<int nSamples>
	std::ofstream & 
		operator<<(std::ofstream & fs,const CoefficientSpectrum<nSamples> & coe)
	{
		for (auto i = 0; i < nSamples; i++)
			fs << coe.c[i] << " ";
		return fs;
	}

	template<int nSamples>
	std::ifstream & 
		operator>>(std::ifstream & fs,const CoefficientSpectrum<nSamples> & coe)
	{
		for (auto i = 0; i < nSamples; i++)
			fs >> coe.c[i];
		return fs;
	}

	template<int nSamples>
	std::ostream & 
		operator<<(std::ostream & os, const CoefficientSpectrum<nSamples> & coe)
	{
		for (auto i = 0; i < nSamples; i++)
			os << coe.c[i]<<" ";
		os << std::endl;
		return os;
	}

	template<int nSamples>
	std::istream & 
		operator>>(std::istream & is, const CoefficientSpectrum<nSamples> & coe)
	{
		for (auto i = 0; i < nSamples; i++)
			is >> coe.c[i];
		return is;
	}


	//class RGBSpectrum: public CoefficientSpectrum<3>
	//{
	//public:
	//	explicit RGBSpectrum(Float v = 0.f):CoefficientSpectrum<3>(v){}
	//	explicit RGBSpectrum(const Float * rgb) 
	//	{
	//		c[0] = rgb[0];
	//		c[1] = rgb[1];
	//		c[2] = rgb[2];
	//	}
	//	void ToRGB(Float * rgb) 
	//	{
	//		rgb[0] = c[0];
	//		rgb[1] = c[1];
	//		rgb[2] = c[2];
	//	}
	//};

	//class RGBASpectrum:public CoefficientSpectrum<4>
	//{
	//public:
	//	explicit RGBASpectrum(Float v = 0.f):CoefficientSpectrum<4>(v){}
	//	explicit RGBASpectrum(const Float * rgba)
	//	{
	//		c[0] = rgba[0];
	//		c[1] = rgba[1];
	//		c[2] = rgba[2];
	//		c[3] = rgba[3];
	//	}
	//	void ToRGBA(Float * rgba)
	//	{
	//		rgba[0] = c[0];
	//		rgba[1] = c[1];
	//		rgba[2] = c[2];
	//		rgba[3] = c[3];
	//	}
	//};

	using RGBASpectrum = CoefficientSpectrum<4>;
	using RGBSpectrum = CoefficientSpectrum<3>;
	
}

#endif

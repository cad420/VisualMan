

#ifndef _INTERPULATOR_H_
#define _INTERPULATOR_H_
#include "../mathematics/basetype.h"
#include <string>
#include "../core/spectrum.h"
#include <vector>

namespace ysl
{

	class MappingKey
	{
	public:
		Float intensity;
		RGBASpectrum leftColor;
		RGBASpectrum rightColor;
		MappingKey(Float intensity, const RGBASpectrum & lc, const RGBASpectrum & rc) :intensity(intensity), leftColor(lc), rightColor(rc) {}
		Float Intensity()const { return intensity; }
		RGBASpectrum LeftColor()const { return leftColor; }
		RGBASpectrum RightColor()const { return rightColor; }
		void SetIntensity(Float intensity) { intensity = intensity; }

		void SetLeftColor(const RGBASpectrum & c)
		{
			const auto a = leftColor.c[3];
			leftColor = c;
			leftColor.c[3] = a;
		}

		void SetRightColor(const RGBASpectrum & c)
		{
			const auto a = leftColor.c[3];
			rightColor = c;
			leftColor.c[3] = a;
		}
		void SetLeftAlpha(Float a) { leftColor.c[3] = a; }
		void SetRightAlpha(Float a) { rightColor.c[3] = a; }
	};


	class ColorInterpulator
	{
	public:
		ColorInterpulator() :m_valid(false) {}
		explicit ColorInterpulator(const std::string & fileName) :m_valid(false)
		{
			read(fileName);
		}

		void read(const std::string& fileName);
		bool valid()const
		{
			return m_valid;
		}
		void FetchData(RGBASpectrum* transferFunction, int dimension)const;
		void FetchData(Float * transferFunction, int dimension)const
		{
			FetchData(reinterpret_cast<RGBASpectrum*>(transferFunction), dimension);
		}
		int KeysCount()const
		{
			return keys.size();
		}
		//const MappingKey & Key()const
		//{
		//	
		//}

		const MappingKey & operator[](int i)const
		{
			return keys[i];
		}

		MappingKey & operator[](int i)
		{
			return keys[i];
		}

	private:
		bool m_valid;
	protected:
		std::vector<MappingKey> keys;
		Float leftThreshold;
		Float rightThreshold;
	};

}

#endif
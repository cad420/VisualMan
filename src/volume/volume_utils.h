#ifndef _VOLUME_UTILS_H_
#define _VOLUME_UTILS_H_

#include "../memory/blockarray.h"
#include "../mathematics/geometry.h"
#include "../core/spectrum.h"
#include "../utility/error.h"
#include "../mathematics/numeric.h"
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <functional>
#include "appearance.h"



struct LVDFileHeader
{
	uint32_t magicNumber;	//  4
	uint32_t vSize[3];		//	12
	uint32_t oSize[3];		//  12
	uint32_t repeat;		//  4
	uint32_t logBlockSize;	//  4
	char reserved[28];		// 28	 == 64bytes in total
};

//#define WRITE_SINGLE_BLOCK

//struct LVDFileHeader
//{
//	uint32_t magicNumber;	//  4
//	uint32_t vSize[3];		//	12
//	uint32_t oSize[3];		//  12
//	uint32_t repeat;		//  4
//	uint32_t logBlockSize;	//  4
//	char reserved[28];		// 28	 == 64bytes in total
//};


/**
 * \brief This should be designed as a boundary-repeated block reader
 */




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


	class TransferFunction:public ColorInterpulator
	{
	public:
		using Callback = std::function<void(TransferFunction* tf)>;
		TransferFunction() = default;
		TransferFunction(const std::string & fileName);
		~TransferFunction();
		void AddMappingKey(const MappingKey & key);
		void UpdateMappingKey(const Point2f & at, const Point2f & to,const Vector2f & scale = {1.0f,1.0f});
		void UpdateMappingKey(const Point2f & at, const MappingKey & key,const Vector2f & scale = {1.0f,1.0f});
		void AddUpdateCallBack(const Callback & func);
		Point2f KeyPosition(int i, Float sx, Float sy)const;
		int HitAt(const Point2f & at, const Vector2f & scale);
		void UpdateMappingKey(int index, const RGBASpectrum& color);
	private:
		std::vector<Callback> callbacks;
		std::function<bool(const MappingKey & m1, const MappingKey & m2)> compareFunc;
		void Notify();
		static Point2f TransToPos(const MappingKey & key);
	};

}


#endif

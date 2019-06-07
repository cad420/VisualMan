

#include "interpulator.h"
#include <iostream>

namespace ysl
{
	void ColorInterpulator::read(const std::string& fileName)
	{
		FILE* fp = fopen(fileName.c_str(), "r");
		if (!fp)
		{
			m_valid = false;
			return;
		}
		int keyNum;
		fscanf(fp, "%d %f %f\n", &keyNum, &leftThreshold, &rightThreshold);
		
		keys.reserve(keyNum);

		float intensity;
		int rl, gl, bl, al, rr, gr, br, ar;
		Float rgba1[4], rgba2[4];
		for (auto i = 0; i < keyNum; ++i)
		{
			fscanf(fp, "%f %d %d %d %d %d %d %d %d\n", &intensity,
				&rl, &gl, &bl, &al, &rr, &gr, &br, &ar);
			rgba1[0] = rl / 255.0f;
			rgba1[1] = gl / 255.0f;
			rgba1[2] = bl / 255.0f;
			rgba1[3] = al / 255.0f;
			rgba2[0] = rr / 255.0f;
			rgba2[1] = gr / 255.0f;
			rgba2[2] = br / 255.0f;
			rgba2[3] = ar / 255.0f;

			const auto s1 = ysl::RGBASpectrum{ rgba1 };
			const auto s2 = ysl::RGBASpectrum{ rgba2 };

			std::cout <<s1 << " " << s2 << std::endl;
			keys.emplace_back(intensity,s1,s2);
		}
		m_valid = true;
		fclose(fp);
	}

	void ColorInterpulator::FetchData(RGBASpectrum* transferFunction, int dimension)const
	{
		const auto frontEnd = static_cast<int>(std::floor(leftThreshold * dimension + 0.5));
		const auto backStart = static_cast<int>(std::floor(rightThreshold * dimension + 0.5));
		//all values before front_end and after back_start are set to zero
		//all other values remain the same
		for (int x = 0; x < frontEnd; ++x)
			transferFunction[x] = RGBASpectrum{ 0.f };

		auto keyIterator = keys.begin();
		RGBASpectrum color;
		for (auto x = frontEnd; x < backStart; ++x)
		{
			const auto value = static_cast<float>(x) / (dimension - 1);
			while ((keyIterator != keys.end()) && (value > (*keyIterator).Intensity()))
				++keyIterator;
			if (keyIterator == keys.begin())
			{
				color = keys[0].LeftColor();
			}
			else if (keyIterator == keys.end())
			{
				color = (*(keyIterator - 1)).RightColor();
			}
			else
			{
				// calculate the value weighted by the destination to the next left and right key
				const auto& leftKey = *(keyIterator - 1);
				const auto& rightKey = *keyIterator;
				const auto fraction = (value - leftKey.Intensity()) / (rightKey.Intensity() - leftKey.Intensity());
				const auto leftDest = leftKey.RightColor();
				const auto rightDest = rightKey.LeftColor();
				color = leftDest + (rightDest - leftDest) * fraction;
			}
			//if (factor != 1)
			//	a = 1.0 - pow(1.f - a, factor);
			transferFunction[x] = color;
		}
		for (int x = backStart; x < dimension; ++x)
			transferFunction[x] = RGBASpectrum{ 0.f };


	}
}


#include <iostream>
#include <sstream>
#include <fstream>
#include "abcflowgen.h"

#include "../mathematics/numeric.h"

int ABCFlowGen()
{
	const auto A = std::sqrt(3);
	const auto B = std::sqrt(2);
	const auto C = 1;



	size_t sideX = 302;
	size_t sideY = 302;
	size_t sideZ = 302;

	//std::cin >> sideX >> sideY >> sideZ;

	std::cout << "Total size:" << sideX * sideY * sideZ << std::endl;

	std::string suffix;
	std::stringstream ss;
	ss << "_" << sideX << "_" << sideY << "_" << sideZ;


	//auto minValue = std::numeric_limits<double>::max();
	//auto maxValue = std::numeric_limits<double>::min();

	constexpr auto minValue = 0.0031238;
	constexpr auto maxValue = 3.4641;

	//std::unique_ptr<char[]> buffer1(new char[sideX*sideY*sideZ]);
	char * buffer1 = (char*)malloc(sideZ*sideY*sideX * sizeof(char));

	//ysl::Block3DArray<char, 5> converter(sideX, sideY, sideZ, buffer1);


	//std::unique_ptr<double[]> buffer0(new double[sideX*sideY*sideZ]);
	//for (size_t z = 0; z < sideZ; z++)
	//{
	//	for (size_t y = 0; y < sideY; y++)
	//	{
	//		for (size_t x = 0; x < sideX; x++)
	//		{
	//			const auto index = x + y * sideX + z * sideX*sideY;
	//			const double X = x * 2 * Pi / sideX, Y = y * 2 * Pi / sideY, Z = z * 2 * Pi / sideZ;
	//			const auto value = std::sqrt(6 + 2 * A *std::sin(Z)*std::cos(Y) + 2 * B * std::sin(Y)*std::cos(X) + 2 * std::sqrt(6)*sin(X)*std::cos(Z));
	//			buffer0[index] = value;
	//			//if (value > maxValue)maxValue = value;
	//			//if (value < minValue)minValue = value;
	//		}
	//	}
	//	std::cout << z << " of " << sideZ << " in total.\n";
	//}

	//const auto w = maxValue - minValue;

	//std::cout << "min value:" << minValue << " " << maxValue << std::endl;

	if (buffer1 == nullptr)
	{
		std::cout << "bad alloc.\n";
		system("pause");
		return 0;
	}

	/*if (converter.Valid() == false)
	{
		std::cout << " converter bad alloc.\n";
		system("pause");
		return 0;
	}*/

#pragma omp parallel for	
	for (int z = 0; z < sideZ; z++)
	{
		for (int y = 0; y < sideY; y++)
		{
			for (int x = 0; x < sideX; x++)
			{
				const auto index = x + y * sideX + z * sideX*sideY;
				const double X = x * 2 * ysl::Pi / sideX, Y = y * 2 * ysl::Pi / sideY, Z = z * 2 * ysl::Pi / sideZ;
				const auto value = std::sqrt(6 + 2 * A *std::sin(Z)*std::cos(Y) + 2 * B * std::sin(Y)*std::cos(X) + 2 * std::sqrt(6)*sin(X)*std::cos(Z));
				buffer1[index] = (value - minValue) / (maxValue - minValue) * 256;
			}
		}
		std::cout << z << " of " << sideZ << " in total.\n";
	}


	std::ofstream outFile("D:\\scidata\\abc\\s1" + ss.str() + ".raw", std::ostream::binary);
	if (outFile.is_open() == false)
	{
		std::cout << " Can not open file\n";
		return 0;
	}

	outFile.write(buffer1, sideZ*sideY*sideX * sizeof(char));
	free(buffer1);


	return 0;
}


#include "../io/rawreader.h"
#include "../mathematics/numeric.h"
#include <string>
#include "../io/platform/windowsfilemap.h"
#include <iostream>

//#include "../io/rawio.h"

namespace ysl
{

	template<typename T>
	class Sampler3D
	{
		T * const data;
		const Size3 size;
	public:
		Sampler3D(T * data,const ysl::Size3 size):data(data),size(size)
		{
			
		}
		ysl::Float Sample(const ysl::Point3i & p)
		{
			ysl::Bound3i bound(ysl::Point3i(0, 0, 0), ysl::Point3i(size.x,size.y,size.z));
			if (!bound.InsideEx(p))
				return 0;
			return (*this)(p.x, p.y, p.z);
		}

		Float Sample(const Point3f & p)
		{
			const auto pi = Point3i(std::floor(p.x), std::floor(p.y), std::floor(p.z));
			const auto d = p - static_cast<Point3f>(pi);
			const auto d00 = Lerp(d.x, Sample(pi), Sample(pi + Vector3i(1, 0, 0)));
			const auto d10 = Lerp(d.x, Sample(pi + Vector3i(0, 1, 0)), Sample(pi + Vector3i(1, 1, 0)));
			const auto d01 = Lerp(d.x, Sample(pi + Vector3i(0, 0, 1)), Sample(pi + Vector3i(1, 0, 1)));
			const auto d11 = Lerp(d.x, Sample(pi + Vector3i(0, 1, 1)), Sample(pi + Vector3i(1, 1, 1)));
			const auto d0 = ysl::Lerp(d.y, d00, d10);
			const auto d1 = ysl::Lerp(d.y, d01, d11);
			return ysl::Lerp(d.z, d0, d1);
		}

		T operator()(int x,int y,int z)const
		{
			return data[z * size.y*size.x + y * size.x + x];
		}
	};
}


ysl::Size3 SampleSize(const ysl::Size3 & orignalSize, const ysl::Vector3f & factor)
{
	return ysl::Size3(1.0*orignalSize.x / factor.x, 1.0*orignalSize.y / factor.y, 1.0*orignalSize.z / factor.z);
}

//void OutOfCoreDownsample(const std::string & fileName,
//	const ysl::Size3 & size,
//	int voxelSize,
//	const ysl::Vector3f & factor,
//	const std::string & outFileName)
//{
//	std::shared_ptr<ysl::AbstrRawIO> rm(new ysl::WindowsMappingRawIO(fileName,size,voxelSize));
//	//std::unique_ptr<unsigned char[]> buf(new unsigned char[*size.x*size.y*voxelSize]);
//	std::ofstream outFile(outFileName, std::ios_base::binary);
//
//	const auto sampleSize = SampleSize(size,factor);
//	ysl::Vector3f step(1.0*size.x / sampleSize.x, 1.0*size.y / sampleSize.y, 1.0*size.z / sampleSize.z);
//
//
//	const auto sliceStep = 5;
//
//	const auto bufSize = sliceStep * sampleSize.x * sampleSize.y * voxelSize;
//
//	std::unique_ptr<unsigned char[]> buf(new unsigned char[bufSize]);
//
//	const auto ptr = rm->FileMemPointer(0, size.x*size.y*size.z*voxelSize);
//
//
//
//
//	for (int zz = 0; zz < sampleSize.z; zz+= sliceStep)
//	{
//		std::size_t actualSlice;
//		if (zz + sliceStep >= sampleSize.z)
//			actualSlice = sampleSize.z - zz;
//		else
//			actualSlice = sliceStep;
//
//
//		int sampleZmin = zz * step.z;
//		int sampleZmax = std::ceil(zz * step.z) + sliceStep;
//
//		for(int z = 0 ;z<actualSlice;z++)
//		{
//			
//
//
//			for (int y = 0; y < sampleSize.y; y++)
//			{
//				for (int x = 0; x < sampleSize.x; x++)
//				{
//					const auto index = sampleSize.x*(zz *sampleSize.y + y) + x;
//					//downsampleData[index] = sampler.Sample(ysl::Point3f(xx * step.x, yy * step.y, zz * step.z));
//				}
//			}
//
//
//		}
//
//	}
//
//	rm->DestroyFileMemPointer(ptr);
//
//
//}




void InCoreDownsample(const std::string & fileName,const ysl::Size3 & size,int voxelSize)
{
	
}

int main()
{

	std::size_t x, y, z;
	int sx, sy, sz;
	std::string inFileName,outFileName;
	std::size_t offset;
	std::cout << "[filename(str), offset(std::size_t),  x(int), y(int), z(int),xfactor(int), yfactor(int),zfactor(int), ouputfilename(str)]\n";
	std::cin >> inFileName >>offset>> x >> y >> z >> sx >> sy >> sz>>outFileName;

	const auto sampleSize = ysl::Size3(1.0*x / sx + 0.5, 1.0*y / sy + 0.5, 1.0*z / sz + 0.5); 
	ysl::Vector3f step( 1.0*x / sampleSize.x,1.0*y / sampleSize.y,1.0*z / sampleSize.z );


	std::cout << "Downsample Size:" << sampleSize << std::endl;
	std::cout << "Step:" << step << std::endl;

	//std::unique_ptr<unsigned char> buf(new unsigned char[x*y*z]);
	std::shared_ptr<ysl::AbstraFileMap> rm(new ysl::WindowsFileMapping(inFileName, offset+x*y*z, ysl::WindowsFileMapping::FileAccess::Read,ysl::WindowsFileMapping::MapAccess::ReadOnly));
	const auto ptr = rm->FileMemPointer(0, x*y*z + offset);
	if(!ptr)
	{
		std::cout << "File mapping failed\n";
		return 0;
	}

	ysl::Sampler3D<unsigned char> sampler(reinterpret_cast<unsigned char*>(ptr+offset), { x,y,z });

	//ysl::RawReader reader(inFileName, {x,y,z},1);
	//std::cout << "Reading Raw Data\n";
	//reader.readRegion({ 0,0,0 }, { x,y,z }, buf.get());
	//std::cout << "Reading finished\n";

	const auto sliceStep = 5;
	const auto bytes = sampleSize.x*sampleSize.y*sliceStep;
	std::unique_ptr<unsigned char[]> downsampleData(new unsigned char[bytes]);
	std::ofstream out(outFileName, std::ios::binary);
//#pragma omp parallel for
	for(int zz = 0 ;zz < sampleSize.z;zz+=sliceStep)
	{
		std::size_t actualSlice;
		if (zz + sliceStep >= sampleSize.z) actualSlice = sampleSize.z - zz;

		else actualSlice = sliceStep;

		const auto actualBytes = actualSlice * sampleSize.x * sampleSize.y;

		for(int s = 0;s < actualSlice;s++)
		{
			for (int yy = 0; yy < sampleSize.y; yy++)
			{
				for (int xx = 0; xx < sampleSize.x; xx++)
				{
					const auto index = sampleSize.x*((s)*sampleSize.y + yy) + xx;
					downsampleData[index] = sampler.Sample(ysl::Point3f(xx * step.x, yy * step.y, (zz+s) * step.z));
				}
			}
		}

		out.write(reinterpret_cast<char*>(downsampleData.get()), actualBytes);
		std::cout << "Writing: " << zz << " to " << zz + actualSlice << " of " << actualBytes << " finished\n";
	}
	system("pause");
	return 0;
}

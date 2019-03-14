
#include "../volume/rawreader.h"
#include "../mathematics/numeric.h"
#include <string>

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

int main()
{
	std::size_t x, y, z;
	int sx, sy, sz;
	std::string inFileName,outFileName;
	std::cin >> inFileName >> x >> y >> z >> sx >> sy >> sz>>outFileName;

	const auto sampleSize = ysl::Size3(1.0*x / sx + 0.5, 1.0*y / sy + 0.5, 1.0*z / sz + 0.5); 
	ysl::Vector3f step( 1.0*x / sampleSize.x,1.0*y / sampleSize.y,1.0*z / sampleSize.z );

	std::cout << "Downsample Size:" << sampleSize << std::endl;
	std::cout << "Step:" << step << std::endl;

	std::unique_ptr<unsigned char> buf(new unsigned char[x*y*z]);
	ysl::Sampler3D<unsigned char> sampler(buf.get(), { x,y,z });


	ysl::RawReader reader(inFileName, {x,y,z},1);
	std::cout << "Reading Raw Data\n";
	reader.readRegion({ 0,0,0 }, { x,y,z }, buf.get());
	std::cout << "Reading finished\n";


	const auto bytes = sampleSize.x*sampleSize.y*sampleSize.z;
	std::cout << "Total bytes:" << bytes << std::endl;
	std::unique_ptr<unsigned char[]> downsampleData(new unsigned char[bytes]);


#pragma omp parallel for
	for(int zz = 0 ;zz<sampleSize.z;zz++)
	{
		for(int yy = 0;yy<sampleSize.y;yy++)
		{
			for(int xx = 0 ;xx<sampleSize.x;xx++)
			{
				const auto index =sampleSize.x*( zz *sampleSize.y + yy  )+ xx;
				downsampleData[index] = sampler.Sample(ysl::Point3f(xx * step.x,yy*step.y ,zz * step.z));
			}
		}
	}

	std::cout << "Downsample finished\n";

	std::ofstream out(outFileName, std::ios::binary);
	out.write((char *)downsampleData.get(), bytes);
	std::cout << "Writing finished\n";

	system("pause");
	return 0;
}

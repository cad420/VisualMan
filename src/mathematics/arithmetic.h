#ifndef ARITHMETIC_H_
#define ARITHMETIC_H_

#include <algorithm>
#include "geometry.h"

#include "numeric.h"

/*
	* Sample Functions
	*/

namespace ysl
{

	inline
	Vector3f
	uniformSampleSphere(const Point2f & p)
	{
		const auto y = 1 - 2 * p[0];
		const auto r = std::sqrt(std::max(Float(0), Float(1 - y * y)));
		const auto phi = 2 * Pi*p[1];
		return Vector3f(r*std::cos(phi), y, r*std::sin(phi));
	}

	inline
	Vector3f
	uniformSampleHemiSphere(const Point2f &p)
	{
		Float y = p[0];
		Float sinTheta = std::sqrt(std::max((Float)0, 1 - y * y));
		Float phi = 2 * Pi*p[1];
		return Vector3f(std::cos(phi)*sinTheta, y, std::sin(phi)*sinTheta);
	}

	inline constexpr
	Float
	uniformHemiSpherePdf()
	{
		return 1 / (2 * Pi);
	}

	inline
	Point2f
	uniformSampleDisk(const Point2f &p)
	{
		Float r = std::sqrt(p[0]);
		Float theta = 2 * Pi*p[1];
		return Point2f(r*std::sin(theta), r*std::cos(theta));
	}

	inline
	Float uniformDiskPdf(Float radius)
	{
		return 1 / (Pi*radius*radius);
	}

	inline
	Vector3f
	cosineSampleHemiSphereWithShiness(const Point2f &p, Float shiness)
	{
		Float phi, theta;
		phi = p[0] * 2 * Pi;
		theta = shiness < 0 ? asin(sqrt(p[1])) : acos(pow(p[1], 1 / (shiness + 1)));
		return Vector3f(sin(theta)*cos(phi), cos(theta), sin(theta)*sin(phi));
	}

	inline
	Vector3f
	cosineSampleHemiSphere(const Point2f &p)
	{
		Point2f sampleFromDisk = uniformSampleDisk(p);
		Float y = std::sqrt(std::max(Float(0), 1 - sampleFromDisk[0] * sampleFromDisk[0] - sampleFromDisk[1] * sampleFromDisk[1]));
		return Vector3f(sampleFromDisk[0], y, sampleFromDisk[2]);
	}

	inline
	Float cosineSampleHemiSpherePdf(Float cosTheta)
	{
		return cosTheta / Pi;
	}


	inline
	Vector3f 
	uniformSampleCone(const Point2f &p, Float angle)
	{
		Float cosTheta = (Float(1) - p[0]) + p[0] * angle;
		Float sinTheta = std::sqrt(Float(1) - cosTheta * cosTheta);
		Float phi = p[1] * 2 * Pi;
		return Vector3f(std::cos(phi)*sinTheta, cosTheta, std::sin(phi)*sinTheta);
	}

	inline
	bool
	russianRoulette(Float p, Float s) {
		if (s <= p)
			return true;
		return false;
	}

	inline
	Vector3f
	reflection(const Vector3f & normal, const Vector3f & incidence)
	{
		Vector3f norm = normal.Normalized();
		Vector3f inci = incidence.Normalized();
		Float s = Vector3f::Dot(norm, inci);
		if (s > 0)return (inci*std::sqrt(1 - s * s)).Normalized();
		return (inci - 2 * norm*(s));
	}

	inline
	Vector3f refraction(const Vector3f& normal, const Vector3f& incidence, Float ratioIOR);

	inline 
	Float 
	clamp(Float v, Float low, Float high) {
		if (v > high)return high;
		if (v < low)return low;
		return v;
	}

	inline
	RGBASpectrum
	clamp(const RGBASpectrum & v,const RGBASpectrum & low,const RGBASpectrum & high)
	{
		RGBASpectrum s;
		s.c[0] = clamp(v.c[0], low.c[0], high.c[0]);
		s.c[1] = clamp(v.c[1], low.c[1], high.c[1]);
		s.c[2] = clamp(v.c[2], low.c[2], high.c[2]);
		s.c[3] = clamp(v.c[3], low.c[3], high.c[3]);
		return s;
	}




	inline
	Float uniformSampleConePdf(Float angle)
	{
		return 1 / (2 * Pi*(1 - angle));
	}

	inline
	Point2f uniformSampleTriangle(const Point2f &p)
	{
		Float su0 = std::sqrt(p[0]);
		return Point2f(1 - su0, p[1] * su0);
	}


	inline 
	int 
	findMaxVector3fComponent(const Vector3f & v)
	{
		int idx = 0;
		Float cMax = v[0];
		for (int i = 1; i < 3; i++)
		{
			if (cMax < v[i])
			{
				cMax = v[i];
				idx = i;
			}
		}
		return idx;
	}

	inline 
	int 
	findMaxPoint3fComponent(const Point3f & p)
	{
		int idx = 0;
		Float cMax = p[0];
		for (int i = 1; i < 3; i++)
		{
			if (cMax < p[i])
			{
				cMax = p[i];
				idx = i;
			}
		}
		return idx;
	}
	inline 
	Vector3f 
	absOfVector3f(const Vector3f & v)
	{
		return Vector3f(std::abs(v[0]), std::abs(v[1]), std::abs(v[2]));
	}

	inline 
	Point3f 
	absOfPoint3f(const Point3f & p)
	{
		return Point3f(std::abs(p[0]), std::abs(p[1]), std::abs(p[2]));
	}

	inline
	Vector3f
	permuteVector3f(const Vector3f & v, int kx, int ky, int kz)
	{
		assert(kx >= 0 && kx < 3);
		assert(ky >= 0 && ky < 3);
		assert(kz >= 0 && kz < 3);
		assert((kx != ky) && (ky != kz) && (kz != kx));
		return Vector3f(v[kx], v[ky], v[kz]);
	}

	inline
	Point3f
	permutePoint3f(const Point3f & p, int kx, int ky, int kz)
	{
		assert(kx >= 0 && kx < 3);
		assert(ky >= 0 && ky < 3);
		assert(kz >= 0 && kz < 3);
		assert((kx != ky) && (ky != kz) && (kz != kx));
		return Point3f(p[kx], p[ky], p[kz]);
	}

	
	void
	rungeKutta4(Point3f* x, Vector3f* v, Vector3f acc, Float dt);

	inline
	void 
	integrateEuler(Point3f * x, Vector3f * v, Vector3f acc, float dt) {
		*x = *x + *v * dt;
		*v = 0.9f* (*v + acc * dt);
	}

	
	bool
	quadraticEquation(Float a, Float b, Float c, Float& t1, Float& t2);
}
#endif
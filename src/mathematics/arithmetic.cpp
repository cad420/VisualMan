#include "arithmetic.h"

namespace ysl
{
	Vector3f refraction(const Vector3f& normal, const Vector3f& incidence, Float ratioIOR)
	{
		const auto norm = normal.Normalized();
		const auto inci = incidence.Normalized();
		Float cosI = Vector3f::Dot(norm, inci);
		Float sinI = std::sqrt(1 - cosI * cosI);


		Float cos2T = 1.0 - (1 - cosI * cosI) * (ratioIOR * ratioIOR);
		if (cos2T < 0)return Vector3f();
		Vector3f T = ratioIOR * inci - (ratioIOR * cosI + std::sqrt(cos2T)) * norm;
		//if (cos2T > 0)return T;
		//return Vector3f();
		return T;
	}

	void rungeKutta4(Point3f* x, Vector3f* v, Vector3f acc, Float dt)
	{
		Point3f p1 = *x;
		Vector3f v1 = *v;
		Vector3f a1 = acc;

		Point3f p2 = *x + 0.5 * v1 * dt;
		Vector3f v2 = *v + 0.5 * a1 * dt;
		Vector3f a2 = acc;

		Point3f p3 = *x + 0.5 * v2 * dt;
		Vector3f v3 = *v + 0.5 * a2 * dt;
		Vector3f a3 = acc;

		Point3f p4 = *x + v3 * dt;
		Vector3f v4 = *v + a3 * dt;
		Vector3f a4 = acc;

		Point3f xfinal = *x + (dt / 6.0) * (v1 + 2 * v2 + 2 * v3 + v4);
		Vector3f vfinal = *v + (dt / 6.0) * (a1 + 2 * a2 + 2 * a3 + a4);

		*x = xfinal;
		*v = 0.9f * (vfinal);
	}

	bool quadraticEquation(Float a, Float b, Float c, Float& t1, Float& t2)
	{
		if (a == 0)return false;
		const auto delta = b * b - 4 * a * c;
		if (delta < 0)return false;
		const auto inv = 1 / (2 * a);
		const auto rd = std::sqrt(delta);
		t1 = (-b + rd) * inv;
		t2 = (-b - rd) * inv;
		return true;
	}
}


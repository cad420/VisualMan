#include "plane.h"

namespace ysl {
	bool ysl::Plane::Intersect(const Ray& ray, Float* t) const
	{
		const auto f = Dot(ray.d, mNormal);
		if (std::abs(f) <= 0.00001)return false;
		if (t != nullptr)  *t = (mOrigin - Dot(mNormal,ray.o))/f;
		return true;
	}

	Float Plane::GetDistance(const Point3f& pos)
	{
		return std::abs(Dot(mNormal,pos) - mOrigin) / mNormal.Length();
	}

}


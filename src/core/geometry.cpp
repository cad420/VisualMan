#include "geometry.h"

using namespace hsl;

bool AABB::intersect(const Ray & ray, hsl::Float * hit0 /*= nullptr*/, hsl::Float * hit1 /*= nullptr*/) const noexcept
{

	auto t1 = ray.m_tMax;

	auto t0 = 0.0;

    for (auto i = 0; i < 3; i++)
    {
	    const auto inv = 1 / ray.m_d[i];
	    auto tNear = (m_min[i] - ray.m_o[i])*inv;
	    auto tFar = (m_max[i] - ray.m_o[i])*inv;
        if (tNear > tFar) std::swap(tNear, tFar);
        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar < t1 ? tFar : t1;
        if (t0 > t1)return false;
    }
    if (hit0 != nullptr)*hit0 = t0;
    if (hit1 != nullptr)*hit1 = t1;
    return true;
}



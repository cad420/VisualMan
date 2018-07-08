#include "geometry.h"

bool AABB::intersect(const Ray & ray, Float * hit0 /*= nullptr*/, Float * hit1 /*= nullptr*/) const noexcept
{
    Float t1 = ray.m_tMax;
    Float t0 = 0;
    for (int i = 0; i < 3; i++)
    {
        Float inv = 1 / ray.m_d[i];
        Float tNear = (m_min[i] - ray.m_o[i])*inv;
        Float tFar = (m_max[i] - ray.m_o[i])*inv;
        if (tNear > tFar)std::swap(tNear, tFar);
        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar < t1 ? tFar : t1;
        if (t0 > t1)return false;
    }
    if (hit0 != nullptr)*hit0 = t0;
    if (hit1 != nullptr)*hit1 = t1;
    return true;
}



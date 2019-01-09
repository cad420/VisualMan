#include "bound.h"
#include "ray.h"

namespace ysl
{
	AABB::AABB() : m_min(MAX_Float_VALUE, MAX_Float_VALUE, MAX_Float_VALUE),
		m_max(LOWEST_Float_VALUE, LOWEST_Float_VALUE, LOWEST_Float_VALUE)
	{
		//construct a empty bounding box
	}

	AABB::AABB(const Point3f& p0, const Point3f& p1) noexcept :
		m_min((std::min)(p0.x, p1.x), (std::min)(p0.y, p1.y), (std::min)(p0.z, p1.z)),
		m_max((std::max)(p0.x, p1.x), (std::max)(p0.y, p1.y), (std::max)(p0.z, p1.z))
	{
	}

	AABB::AABB(const Point3f& p) : m_min(p), m_max(p)
	{
	}

	bool AABB::Intersect(const Ray & ray, ysl::Float * hit0 /*= nullptr*/, ysl::Float * hit1 /*= nullptr*/) const noexcept
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

	Point3f
		AABB::Center() const
	{
		return (m_min + m_max) / 2.0;
	}

	Vector3f
		AABB::Diagonal() const
	{
		return m_max - m_min;
	}


	Float
		AABB::SurfaceArea() const
	{
		const auto d = Diagonal();
		if (d[0] < 0 || d[1] < 0 || d[2] < 0)return Float(0);
		const auto area = (d[0] * d[1] + d[1] * d[2] + d[2] * d[0]) * 2;
		return area;
	}

	bool
		AABB::IsIntersectWith(const AABB & b)const
	{
		return (m_max.x >= b.m_min.x && b.m_max.x >= m_min.x) &&
			(m_max.y >= b.m_min.y && b.m_max.y >= m_min.y) &&
			(m_max.z >= b.m_min.z && b.m_max.z >= m_min.z);
	}

	AABB
		AABB::UnionWith(const Point3f & p)const {
		AABB a;
		a.m_min = Point3f(
			(std::min)(m_min.x, p.x),
			(std::min)(m_min.y, p.y),
			(std::min)(m_min.z, p.z)
		);
		a.m_max = Point3f(
			(std::max)(m_max.x, p.x),
			(std::max)(m_max.y, p.y),
			(std::max)(m_max.z, p.z)
		);
		return a;
	}

	AABB
		AABB::UnionWith(const AABB & b) const
	{
		AABB a;
		a.m_min = Point3f{
			(std::min)(m_min.x, b.m_min.x),
			(std::min)(m_min.y, b.m_min.y),
			(std::min)(m_min.z, b.m_min.z)
		};
		a.m_max = Point3f{
			(std::max)(m_max.x, b.m_max.x),
			(std::max)(m_max.y, b.m_max.y),
			(std::max)(m_max.z, b.m_max.z)
		}; // For fucking min/max defined in windows.h
		return a;
	}

	AABB
		AABB::IntersectWidth(const AABB & b) const
	{
		return AABB{ Point3f{(std::max)(m_min.x, b.m_min.x),
			(std::max)(m_min.y, b.m_min.y),
			(std::max)(m_min.z, b.m_min.z)},
			Point3f{(std::min)(m_max.x, b.m_max.x),
				(std::min)(m_max.y, b.m_max.y),
				(std::min)(m_max.z, b.m_max.z)}
		}; // For fucking min/max defined in windows.h
	}

}
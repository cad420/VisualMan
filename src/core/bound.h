
#ifndef _BOUND_H_
#define _BOUND_H_

#include "../mathematics/geometry.h"


namespace ysl
{
	class Ray;

	class AABB
	{
		Point3f m_min;
		Point3f m_max;
	public:
		AABB();

		AABB(const Point3f & p0, const Point3f & p1)noexcept;

		explicit AABB(const Point3f& p);

		const Point3f & operator[](int i)const
		{
			assert(i >= 0 && i < 2);
			if (i == 0)return m_min;
			if (i == 1)return m_max;
			return Point3f{};
		}

		Point3f & operator[](int i)
		{
			return const_cast<Point3f&>(static_cast<const AABB &>(*this)[i]);
		}

		Point3f Corner(int i)const
		{
			return Point3f{ (*this)[i & 1].x, (*this)[i & 2].y, (*this)[i & 4].z };
		}

		/*
		* Check whether it is intersected with a ray.
		* hit0 stores the nearest ray
		* parameter and hit1 stores the farthest ray parameter
		*/

		/*inline function definitions for AABB*/

		bool Intersect(const Ray & ray, Float * hit0 = nullptr, Float * hit1 = nullptr) const noexcept;

		Point3f Center() const;

		Vector3f Diagonal() const;

		Float SurfaceArea() const;

		/*
		*Check whether a point is in the bound
		*/

		bool Inside(const Point3f & p) const
		{
			return (p.x >= m_min.x && p.y <= m_max.x &&
				p.y >= m_min.y && p.y <= m_max.y &&
				p.z >= m_min.z && p.z <= m_max.z);
		}

		bool InsideEx(const Point3f & p) const
		{
			return (p.x >= m_min.x && p.y < m_max.x &&
				p.y >= m_min.y && p.y < m_max.y &&
				p.z >= m_min.z && p.z < m_max.z);
		}

		/*
		* Check whether the bounding box is
		* intersected with another bounding box
		*/

		bool IsIntersectWith(const AABB & b)const;

		/*
		* return the common part of two bounding box
		*/

		AABB IntersectWidth(const AABB & b) const;

		/*
		* Return a minimum bounding box containing the two bounding boxes
		*/
		AABB UnionWith(const AABB & b)const;
		/*
		* Return a minimun bounding box containing the
		* bounding box and the point
		*/
		AABB UnionWith(const Point3f & p)const;

		friend class BVHTreeAccelerator;
	};
}

#endif

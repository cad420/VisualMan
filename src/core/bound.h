
#ifndef _BOUND_H_
#define _BOUND_H_

#include "../mathematics/geometry.h"



namespace ysl
{
	class Ray;

	class Bound3i
	{
	public:
		Point3i min;
		Point3i max;
		Bound3i();

		Bound3i(const Point3i & p0, const Point3i & p1)noexcept:min(p0),max(p1){
		}

		explicit Bound3i(const Point3i& p):min(p),max(p){

		}

		const Point3i & operator[](int i)const
		{
			assert(i >= 0 && i < 2);
			if (i == 0)return min;
			if (i == 1)return max;
		}

		Point3i & operator[](int i)
		{
			return const_cast<Point3i&>(static_cast<const Bound3i &>(*this)[i]);
		}

		Point3i Corner(int i)const
		{
			return Point3i{ (*this)[i & 1].x, (*this)[i & 2].y, (*this)[i & 4].z };
		}

		Vector3i Diagonal() const
		{
			return max-min;
		}

		int Width()const
		{
			return max.x-min.x;
		}

		int Height()const
		{
			return max.y-min.y;
		}

		int Depth()const
		{
			return max.z- min.z;
		}

		Float SurfaceArea()const{
			const auto d = Diagonal();
			if (d[0] < 0 || d[1] < 0 || d[2] < 0)return Float(0);
			const auto area = (d[0] * d[1] + d[1] * d[2] + d[2] * d[0]) * 2;
			return area;
		}


		/*
		*Check whether a point is in the bound
		*/

		bool Inside(const Point3f & p) const
		{
			return (p.x >= min.x && p.y <= max.x &&
					p.y >= min.y && p.y <= max.y &&
					p.z >= min.z && p.z <= max.z);
		}

		bool InsideEx(const Point3f & p) const
		{
			return (p.x >= min.x && p.y < max.x &&
					p.y >= min.y && p.y < max.y &&
					p.z >= min.z && p.z < max.z);
		}


	};



	class Bound3f
	{
		Point3f m_min;
		Point3f m_max;
	public:
		Bound3f();

		Bound3f(const Point3f & p0, const Point3f & p1)noexcept;

		explicit Bound3f(const Point3f& p);

		const Point3f & operator[](int i)const
		{
			assert(i >= 0 && i < 2);
			if (i == 0)return m_min;
			if (i == 1)return m_max;
		}

		Point3f & operator[](int i)
		{
			return const_cast<Point3f&>(static_cast<const Bound3f &>(*this)[i]);
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

		bool IsIntersectWith(const Bound3f & b)const;

		/*
		* return the common part of two bounding box
		*/

		Bound3f IntersectWidth(const Bound3f & b) const;

		/*
		* Return a minimum bounding box containing the two bounding boxes
		*/
		Bound3f UnionWith(const Bound3f & b)const;
		/*
		* Return a minimun bounding box containing the
		* bounding box and the point
		*/
		Bound3f UnionWith(const Point3f & p)const;

		friend class BVHTreeAccelerator;
	};




}

#endif

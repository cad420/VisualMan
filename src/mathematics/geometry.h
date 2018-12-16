#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iostream>
#include <cassert>
#include <cmath>
#include <algorithm>

#include "basetype.h"
#include "numeric.h"


namespace ysl
{


	template <class T> class Vector2;
	template <class T> class Vector3;
	template <class T> class Point3;
	template <class T> class Point2;

	typedef Vector3<Float> Vector3f;
	typedef Vector3<int> Vector3i;
	typedef Point3<Float> Point3f;
	typedef Point3<int> Point3i;

	typedef Point2<Float> Point2f;
	typedef Vector2<Float> Vector2f;
	typedef Point2<int> Point2i;
	typedef Vector2<int> Vector2i;

	template<typename T>
	class Vector2 {
	public:
		T x, y;
		Vector2() :x(0), y(0) {}
		Vector2(const T & _x, const T & _y) :x(_x), y(_y) {}
		bool HasNaN()const
		{
			return IsNaN(x) || IsNaN(y);
		}

		Vector2<T> operator+(const Vector2<T> & p)const
		{
			assert(!p.HasNaN());
			return Vector2<T>(x + p.x, y + p.y);
		}

		Point2<T> operator+(const Point2<T> & v)const
		{
			assert(!v.HasNaN());
			return Point2<T>(x + v.x, y + v.y);
		}

		Vector2<T> & operator+=(const Vector2<T> & v) {
			assert(!v.HasNaN());
			x += v.x;
			y += v.y;
			return *this;
		}

		Vector2<T> operator-(const Vector2<T> & v)
		{
			assert(!v.HasNaN());
			return Vector2<T>(x - v.x, y - v.y);
		}

		Vector2<T> & operator-=(const Vector2<T> & v)
		{
			assert(!v.HasNaN());
			x -= v.x;
			y -= v.y;
			return *this;
		}
		template<typename U>
		Vector2<T> operator/(const U & s)const
		{
			assert(!IsNaN(s));
			const Float inv = static_cast<Float>(1) / s;
			return Vector2<T>(x*inv, y*inv);
		}

		template<typename U>
		Vector2<T> & operator/=(const U & s)
		{
			assert(!IsNaN(s));
			const Float inv = static_cast<Float>(1) / s;
			x *= inv; y *= inv;
			return *this;
		}
		template<typename U>
		Vector2<T> operator*(const U & s)
		{
			assert(!IsNaN(s));
			return Vector2<T>(s*x, s*y);
		}

		template<typename U>
		Vector2<T> & operator*=(const U & s)
		{
			assert(!IsNaN(s));
			x *= s;
			y *= s;
			return *this;
		}

		Vector2<T> operator-()
		{
			return Vector2<T>(-x, -y);
		}

		const T & operator[](int i)const
		{
			assert(i >= 0 && i < 2); return *(&x + i);
		}

		T & operator[](int i)
		{
			assert(i >= 0 && i < 2); return *(&x + i);
		}

		bool operator==(const Vector2<T> & v)
		{
			return x == v.x&&y == v.y;
		}
		bool operator!=(const Vector2<T> & v)
		{
			return !(*this == v);
		}

		Float
			LengthSquared()const
		{
			return x * x + y * y;
		}

		Float
			Length()const
		{
			return std::sqrt(LengthSquared());
		}

		Vector2<T> normalized()const
		{
			const Float len = LengthSquared();
			return *this / len;
		}

		void normalize()
		{
			const Float len = LengthSquared();
			*this /= len;
		}

		T * Data()
		{
			return &x;
		}

		const T * ConstData()const
		{
			return &x;
		}

		template<typename X> friend class Point2; //Vector2D can be accessed by all instances of Point2D
	};


	template<typename T>
	std::ostream & operator<<(std::ostream & os, const Vector2<T> & v)
	{
		os << "[" << v.x << ", " << v.y << "]";
		return os;
	}




	/*
	* Point2D
	*/
	template<typename T>
	class Point2 {
	public:
		T x, y;
		Point2() :x(0), y(0) {}
		Point2(const T & x, const T & y) :x(x), y(y)
		{
			assert(!HasNaN());
		}
		//  explicit Point2D(const Point2D<T> & v) :xp(v.xp), yp(v.yp)
		//  {
			  //assert(!HasNaN());
		//  }

		bool HasNaN()const
		{
			return IsNaN(x) || IsNaN(y);
		}

		//template<typename U>
		//explicit operator Vector2<U>()const
		//{
		//	return Vector2<U>(x, y);
		//}

		Point2<T> operator+(const Vector2<T> & v)
		{
			return Point2<T>(x + v.x, y + v.y);
		}
		Point2<T> operator+(const Point2<T> & p)
		{
			return Point2<T>(x + p.x, y + p.y);
		}
		Point2<T> operator+=(const Point2<T> & p)
		{
			return Point2<T>(x + p.x, y + p.y);
		}
		Point2<T> & operator+=(const Vector2<T> & v)
		{
			x += v.x;
			y += v.y;
			return *this;
		}

		Point2<T> operator-(const Vector2<T> & v)
		{
			assert(!v.HasNaN());
			return Point2<T>(x - v.x, y - v.y);
		}
		Vector2<T> operator-(const Point2<T> & p)
		{
			assert(!p.HasNaN());
			return Vector2<T>(x - p.x, y - p.y);
		}
		Point2<T> operator-()
		{
			return Point2<T>(-x, -y);
		}

		Point2<T> & operator-=(const Vector2<T> & v)
		{
			assert(!v.HasNaN());
			x -= v.x; y -= v.y; return *this;
		}
		template<typename U>
		Point2<T> operator*(const U & s)
		{
			assert(!IsNaN(s));

			return Point2<T>(s*x, s*y);
		}
		template<typename U>
		Point2<T> & operator*=(const U & s)
		{
			assert(!IsNaN(s));
			x *= s; y *= s; return *this;
		}
		template<typename U>
		Point2<T> operator/(const U & s)const
		{
			assert(!IsNaN(s));
			const Float inv = static_cast<Float>(1) / s;
			return Point2<T>(x*inv, y*inv);
		}
		template<typename U>
		Point2<T> & operator/=(const U & s)
		{
			assert(!IsNaN(s));
			const Float inv = static_cast<Float>(1) / s;
			x *= inv; y *= inv;
			return *this;
		}
		const T & operator[](int i)const
		{
			assert(i >= 0 && i < 2);
			return *(&x + i);
		}
		T & operator[](int i)
		{
			assert(i >= 0 && i < 2);
			return *(&x + i);
		}

		bool operator==(const Point2<T> & p)
		{
			return x == p.x&&y == p.y;
		}
		bool operator!=(const Point2<T> & p)
		{
			return !(*this == p);
		}

		T * Data()
		{
			return &x;
		}

		const T * ConstData()const
		{
			return &x;
		}

	};

	template<typename T> std::ostream & operator<<(std::ostream & os, const Point2<T> & p)
	{
		os << "[" << p.x << "," << p.y << "]";
		return os;
	}

	template<typename T, typename U> inline Point2<T> operator*(const U & s, const Point2<T> & v)
	{
		return Point2<T>(s*v.x(), s*v.y());
	}


	template<typename T>
	class Vector3 {
	public:
		T x, y, z;
		Vector3() :x(0), y(0), z(0) {}
		Vector3(const T &x, const T &y, const T& z) :x(x), y(y), z(z)
		{
			assert(!HasNaN());
		}
		//Vector3D(const Vector3D<T> & v) :x(T(v.x)), y(T(v.y)), z(T(v.z)) {}
		bool HasNaN()const
		{
			return IsNaN(x) || IsNaN(y) || IsNaN(z);
		}
		Vector3<T> operator+(const Vector3<T> & v)const
		{
			assert(!v.HasNaN());
			return Vector3<T>(x + v.x, y + v.y, z + v.z);
		}

		Point3<T> operator+(const Point3<T> & p)const
		{
			assert(!p.HasNaN());
			return Point3<T>{x+p.x,y+p.y,z+p.z};
		}

		Vector3<T> & operator+=(const Vector3<T> & v) {
			assert(!v.HasNaN());
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}
		Vector3<T> operator-(const Vector3<T> & v)const
		{
			assert(!v.HasNaN());
			return Vector3<T>(x - v.x, y - v.y, z - v.z);
		}

		Vector3<T> & operator-=(const Vector3<T> & v)
		{
			assert(!v.HasNaN());
			x -= v.x;
			y -= v.y;
			z -= v.z;
			return *this;
		}


		//unary operator
		Vector3<T> operator-()const
		{
			return Vector3<T>(-x, -y, -z);
		}

		Vector3<T> operator*(const Vector3<T> & v)const
		{
			return { x*v.x,y*v.y,z*v.z };
		}

		Vector3<T> operator*=(const Vector3<T> & v)
		{
			x *= v.x;
			y *= v.y;
			z *= v.z;
			return *this;
		}

		template<typename U>
		Vector3<T> operator*(const U & s)const
		{
			assert(!IsNaN(s));
			return Vector3<T>(s*x, s*y, s*z);
		}
		template<typename U>
		Vector3<T> & operator*=(const U& s)
		{
			assert(!IsNaN(s));
			x *= s;
			y *= s;
			z *= s;
			return *this;
		}


		Vector3<T> operator/(Float s)const
		{
			assert(!IsNaN(s));
			const auto inv = static_cast<Float>(1) / s;
			return Vector3<T>(x*inv, y*inv, z*inv);
		}

		Vector3<T> &operator/=(Float s)
		{
			assert(!IsNaN(s));
			const auto inv = static_cast<Float>(1) / s;
			x *= inv; y *= inv; z *= inv;
			return *this;
		}

		const T & operator[](int i)const {
			assert(i >= 0 && i < 3);
			return *(&x + i);
		}
		T & operator[](int i) {
			assert(i >= 0 && i < 3);
			return *(&x + i);
		}



		static Float Dot(const Vector3f & v1, const Vector3f & v2)
		{
			return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
		}


		static Vector3f Cross(const Vector3f & v1, const Vector3f & v2)
		{
			return Vector3<T>{
				v1.y * v2.z - v1.z * v2.y,
					v1.z * v2.x - v1.x * v2.z,
					v1.x * v2.y - v1.y * v2.x};
		}

		Float LengthSquared()const { return x * x + y * y + z * z; }

		Float Length()const { return std::sqrt(LengthSquared()); }

		Vector3<T> Normalized()const
		{
			//if len is too small?
			const auto len = Length();
			return (*this) / len;
		}

		void Normalize()
		{
			//if len is too small?
			const auto len = Length();
			(*this) /= len;
		}

		bool isNull()const
		{
			return x == 0 && y == 0 && z == 0;
		}

		T * Data()
		{
			return &x;
		}

		const T * ConstData()const
		{
			return &x;
		}

	};


	template<typename T>
	std::ostream & operator<<(std::ostream & os, const Vector3<T> & v)
	{
		os << "[" << v.x << "," << v.y << "," << v.z << "]";
		return os;
	}


	template<typename T>
	Vector3<T> operator*(Float s, const Vector3<T> & v)
	{
		return v * s;
	}

	template<typename T>
	Vector3<T> Abs(const Vector3<T> & v) {
		return Vector3<T>(std::abs(v.x()), std::abs(v.y()), std::abs(v.z()));
	}


	/*
	* Point3D
	*/

	template<typename T>
	class Point3 {
	public:
		T x, y, z;
		Point3() :x(0), y(0), z(0) {}
		Point3(const T &x, const T &y, const T& z) :x(x), y(y), z(z)
		{
			assert(!HasNaN());
		}

		bool HasNaN()const
		{
			return IsNaN(x) || IsNaN(y) || IsNaN(z);
		}
		//template<typename U> explicit Point3D(const Point3D<U> & p) :x(T(p.x)), y(T(p.y)), z(T(p.z)) {}

		template<typename U>
		explicit operator Vector3<U>()const
		{
			return Vector3<U>(x, y, z);
		}


		Point3<T> operator+(const Point3<T> & p)const
		{
			assert(!p.HasNaN());
			return Point3<T>(x + p.x, y + p.y, z + p.z);
		}

		Point3<T> operator+(const Vector3<T> & v)const
		{
			assert(!v.HasNaN());
			return Point3<T>(x + v.x, y + v.y, z + v.z);
		}

		Point3<T> & operator+=(const Vector3<T> & v)
		{
			assert(!v.HasNaN());
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}

		Point3<T> & operator+=(const Point3<T> & p)
		{
			assert(!p.HasNaN());
			x += p.x;
			y += p.y;
			z += p.z;
			return *this;
		}

		template<typename U>
		Point3<T> operator*(const U & s)const
		{
			assert(!IsNaN(s));
			return Point3<T>(s*x, s*y, s*z);
		}

		//Point3<T> operator*(const Point3<T> & p)const
		//{
		//	assert(!IsNaN(s));
		//	return Point3<T>{x*p.x,y*p.y,z*p.z};
		//}


		template<typename U>
		Point3<T> & operator*=(const U & s)
		{
			assert(!IsNaN(s));
			x *= s;
			y *= s;
			z *= s;
			return *this;
		}

		template<typename U>
		Point3<T> operator/(const U & s)const
		{
			assert(!IsNaN(s));
			const Float inv = static_cast<Float>(1) / s;
			return Point3<T>(inv*x, inv*y, inv*z);
		}

		template<typename U>
		Point3<T> & operator/=(const U & s)
		{
			assert(!IsNaN(s));
			const Float inv = static_cast<Float>(1) / s;
			x *= inv;
			y *= inv;
			z *= inv;
			return *this;
		}

		Vector3<T> operator-(const Point3<T> & p)const
		{
			assert(!p.HasNaN());
			return Vector3<T>(x - p.x, y - p.y, z - p.z);
		}

		Point3<T> operator-(const Vector3<T> & v) const
		{
			assert(!v.HasNaN());
			return Point3<T>(x - v.x, y - v.y, z - v.z);
		}

		Point3<T> operator-()const
		{
			return Point3<T>(-x, -y, -z);
		}

		Point3<T> & operator-=(const Vector3<T> & v)
		{
			assert(!v.HasNaN());
			x -= v.x;
			y -= v.y;
			z -= v.z;
			return *this;
		}

		const T& operator[](int i)const
		{
			assert(i >= 0 && i < 3);
			return *(&x + i);
		}

		T & operator[](int i)
		{
			assert(i >= 0 && i < 3);
			return *(&x + i);
		}


		bool isNull()const
		{
			return x == 0 && y == 0 && z == 0;
		}

		//template<typename T>
		//static Float Dot(const Point3<T> & v1, const Point3<T> & v2)
		//{
		//	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
		//}

		//template<typename T>
		//static Point3<T> Cross(const Point3<T> & v1, const Point3<T> & v2)
		//{
		//	return Point3<T>{
		//		v1.y * v2.z - v1.z * v2.y,
		//			v1.z * v2.x - v1.x * v2.z,
		//			v1.x * v2.y - v1.y * v2.x};
		//}


		Vector3<T> Abs(const Vector3<T> & v)
		{
			return Vector3<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z));
		}

		T * Data()
		{
			return &x;
		}

		const T * ConstData()const
		{
			return &x;
		}

	};

	template<typename T, typename U>
	Point3<T> operator*(const U & s, const Point3<T> & p)
	{
		return p * s;
	}

	template<typename T> std::ostream & operator<<(std::ostream & os, const Point3<T> & p)
	{
		os << "[" << p.x << "," << p.y << "," << p.z << "]";
		return os;
	}



	class Normal3f
	{
	public:

		Float x, y, z;

		Normal3f(Float _x, Float _y, Float _z)
		{

		}

		template<typename T>
		Normal3f(const Vector3<T> & v)
		{

		}
	};

	// Ray

	class Ray
	{
		Point3f m_o;
		Vector3f m_d;
		Float m_tMax;
		bool m_negDirection[3];
	public:
		Ray(const Vector3f& d, const Point3f& o, Float t = MAX_Float_VALUE) noexcept;
		Point3f operator()(float t)const noexcept { return m_o + t * m_d; }
		const Point3f & original()const { return m_o; }
		const Vector3f & direction()const { return m_d; }
		void setMaxLength(Float t) { m_tMax = t; }

		friend class AABB;
		friend class Triangle;
		friend class Shape;
		friend class BVHTreeAccelerator;
	};

	// Axis-Aligned Bounding Box

	class AABB
	{
		Point3f m_min;
		Point3f m_max;
	public:
		AABB();

		AABB(const Point3f & p0, const Point3f & p1)noexcept;

		AABB(const Point3f& p);

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

		Point3f corner(int i)const
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

#endif // GEOMETRY_H
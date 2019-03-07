#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iostream>
#include <cassert>
#include <cmath>
#include <algorithm>

#include "basetype.h"
//#include "numeric.h"


namespace ysl
{

	template<typename T>
	bool
	IsNaN(const T & t)
	{
		return std::isnan(t);
	}

	template<> inline
	bool
	IsNaN(const int & t)
	{
		return false;
	}

	template<> inline
	bool
	IsNaN(const std::size_t & t)
	{
		return false;
	}

	template <class T> class Vector2;
	template <class T> class Vector3;
	template <class T> class Point3;
	template <class T> class Point2;
	template <class T> class Normal3;

	using Vector3f = Vector3<Float>;
	using Vector3i = Vector3<int>;
	using Point3f = Point3<Float>;
	using Point3i = Point3<int>;

	using Vector2f = Vector2<Float>;
	using Vector2i = Vector2<int>;
	using Point2f = Point2<Float>;
	using Point2i = Point2<int>;

	using Size3 = Vector3<std::size_t>;
	using Size2 = Vector2<std::size_t>;

	using Vec3f = Vector3f;
	using Vec3i = Vector3i;
	using Vec2f = Vector2f;
	using Vec2i = Vector2i;

	template<typename T>
	class Vector2 {
	public:
		T x, y;
		constexpr Vector2() :x(0), y(0) {}
		constexpr Vector2(const T & _x, const T & _y) :x(_x), y(_y) {}
		bool HasNaN()const
		{
			return IsNaN(x) || IsNaN(y);
		}

		constexpr Vector2<T> operator+(const Vector2<T> & p)const
		{
			assert(!p.HasNaN());
			return Vector2<T>(x + p.x, y + p.y);
		}

		constexpr Point2<T> operator+(const Point2<T> & v)const
		{
			assert(!v.HasNaN());
			return Point2<T>(x + v.x, y + v.y);
		}

		constexpr Vector2<T> & operator+=(const Vector2<T> & v) {
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

		template<typename U>
		explicit operator Vector2<U>()const
		{
			return Vector2<U>(U(x),U(y));
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

		Vector2<T> Normalized()const
		{
			const Float len = LengthSquared();
			return *this / len;
		}

		void Normalize()
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
		constexpr Point2() :x(0), y(0) {}
		constexpr Point2(const T & x, const T & y) :x(x), y(y)
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

		constexpr Point2<T> operator+(const Vector2<T> & v)const
		{
			return Point2<T>(x + v.x, y + v.y);
		}

		constexpr Point2<T> operator+(const Point2<T> & p)const
		{
			return Point2<T>(x + p.x, y + p.y);
		}

		constexpr Point2<T> operator+=(const Point2<T> & p)const
		{
			return Point2<T>(x + p.x, y + p.y);
		}

		Point2<T> & operator+=(const Vector2<T> & v)
		{
			x += v.x;
			y += v.y;
			return *this;
		}

		Point2<T> operator-(const Vector2<T> & v)const
		{
			assert(!v.HasNaN());
			return Point2<T>(x - v.x, y - v.y);
		}
		Vector2<T> operator-(const Point2<T> & p)const
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

		explicit operator Vector2<T>()const
		{
			return Vector2<T>{x,y};
		}


		template<typename U>
		explicit operator Point2<U>()const
		{
			return Point2<U>(U(x), U(y));
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
		constexpr Vector3() :x(0), y(0), z(0) {}
		constexpr Vector3(const T &x, const T &y, const T& z) :x(x), y(y), z(z)
		{
			//assert(!HasNaN());
		}

		//constexpr Vector3(const Normal3<T> & n):x(n.x),y(n.y),z(n.z){}
		//Vector3D(const Vector3D<T> & v) :x(T(v.x)), y(T(v.y)), z(T(v.z)) {}
		bool HasNaN()const
		{
			return IsNaN(x) || IsNaN(y) || IsNaN(z);
		}

		constexpr Vector3<T> operator+(const Vector3<T> & v)const
		{
			//assert(!v.HasNaN());
			return Vector3<T>(x + v.x, y + v.y, z + v.z);
		}

		constexpr Point3<T> operator+(const Point3<T> & p)const
		{
			//assert(!p.HasNaN());
			return Point3<T>{x+p.x,y+p.y,z+p.z};
		}

		constexpr Vector3<T> & operator+=(const Vector3<T> & v) {
			//assert(!v.HasNaN());
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}
		constexpr Vector3<T> operator-(const Vector3<T> & v)const
		{
			//assert(!v.HasNaN());
			return Vector3<T>(x - v.x, y - v.y, z - v.z);
		}

		constexpr Vector3<T> & operator-=(const Vector3<T> & v)
		{
			//assert(!v.HasNaN());
			x -= v.x;
			y -= v.y;
			z -= v.z;
			return *this;
		}


		template<typename U>
		explicit operator Vector3<U>()const
		{
			return Vector3<U>(U(x), U(y),U(z));
		}


		//unary operator
		constexpr Vector3<T> operator-()const
		{
			return Vector3<T>(-x, -y, -z);
		}

		constexpr Vector3<T> operator*(const Vector3<T> & v)const
		{
			return { x*v.x,y*v.y,z*v.z };
		}

		constexpr Vector3<T> operator*=(const Vector3<T> & v)
		{
			x *= v.x;
			y *= v.y;
			z *= v.z;
			return *this;
		}

		template<typename U>
		constexpr Vector3<T> operator*(const U & s)const
		{
			//assert(!IsNaN(s));
			return Vector3<T>(s*x, s*y, s*z);
		}
		template<typename U>
		constexpr Vector3<T> & operator*=(const U& s)
		{
			//assert(!IsNaN(s));
			x *= s;
			y *= s;
			z *= s;
			return *this;
		}


		constexpr Vector3<T> operator/(Float s)const
		{
			//assert(!IsNaN(s));
			const auto inv = static_cast<Float>(1) / s;
			return Vector3<T>(x*inv, y*inv, z*inv);
		}

		constexpr Vector3<T> &operator/=(Float s)
		{
		//	assert(!IsNaN(s));
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

		constexpr Float LengthSquared()const { return x * x + y * y + z * z; }

		constexpr Float Length()const { return std::sqrt(LengthSquared()); }

		constexpr Vector3<T> Normalized()const
		{
			//if len is too small?
			const auto len = Length();
			return (*this) / len;
		}

		constexpr void Normalize()
		{
			//if len is too small?
			const auto len = Length();
			(*this) /= len;
		}

		constexpr bool IsNull()const
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
	constexpr Vector3<T> operator*(Float s, const Vector3<T> & v)
	{
		return v * s;
	}

	template<typename T>
	constexpr Vector3<T> Abs(const Vector3<T> & v) {
		return Vector3<T>(std::abs(v.x()), std::abs(v.y()), std::abs(v.z()));
	}


	/*
	* Point3D
	*/

	template<typename T>
	class Point3 {
	public:
		T x, y, z;
		constexpr Point3() :x(0), y(0), z(0) {}
		constexpr Point3(const T &x, const T &y, const T& z) :x(x), y(y), z(z)
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

		template<typename U>
		explicit operator Point3<U>()const
		{
			return Point3<U>(U(x), U(y), U(z));
		}

		constexpr Point3<T> operator+(const Point3<T> & p)const
		{
			assert(!p.HasNaN());
			return Point3<T>(x + p.x, y + p.y, z + p.z);
		}

		constexpr Point3<T> operator+(const Vector3<T> & v)const
		{
			assert(!v.HasNaN());
			return Point3<T>(x + v.x, y + v.y, z + v.z);
		}

		constexpr Point3<T> & operator+=(const Vector3<T> & v)
		{
			assert(!v.HasNaN());
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}

		constexpr Point3<T> & operator+=(const Point3<T> & p)
		{
			assert(!p.HasNaN());
			x += p.x;
			y += p.y;
			z += p.z;
			return *this;
		}

		template<typename U>
		constexpr Point3<T> operator*(const U & s)const
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
		constexpr Point3<T> & operator*=(const U & s)
		{
			assert(!IsNaN(s));
			x *= s;
			y *= s;
			z *= s;
			return *this;
		}

		template<typename U>
		constexpr Point3<T> operator/(const U & s)const
		{
			assert(!IsNaN(s));
			const Float inv = static_cast<Float>(1) / s;
			return Point3<T>(inv*x, inv*y, inv*z);
		}

		template<typename U>
		constexpr Point3<T> & operator/=(const U & s)
		{
			assert(!IsNaN(s));
			const Float inv = static_cast<Float>(1) / s;
			x *= inv;
			y *= inv;
			z *= inv;
			return *this;
		}

		constexpr Vector3<T> operator-(const Point3<T> & p)const
		{
			assert(!p.HasNaN());
			return Vector3<T>(x - p.x, y - p.y, z - p.z);
		}

		constexpr Point3<T> operator-(const Vector3<T> & v) const
		{
			assert(!v.HasNaN());
			return Point3<T>(x - v.x, y - v.y, z - v.z);
		}

		constexpr Point3<T> operator-()const
		{
			return Point3<T>(-x, -y, -z);
		}

		constexpr Point3<T> & operator-=(const Vector3<T> & v)
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


		bool IsNull()const
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

	template<typename T>
	class Normal3
	{
	public:
		T x, y, z;
		explicit Normal3<T>(const Vector3<T> & v):x(v.x),y(v.y),z(v.z)
		{
			assert(!v.HasNaN());
		}
		Normal3<T>(T x, T y, T z):x(x),y(y),z(z)
		{

		}


		bool HasNaN()const
		{
			return IsNaN(x) || IsNaN(y) || IsNaN(z);
		}

		constexpr Normal3<T> operator+(const Normal3<T> & v)const
		{
			//assert(!v.HasNaN());
			return Vector3<T>(x + v.x, y + v.y, z + v.z);
		}

		constexpr Normal3<T> & operator+=(const Normal3<T> & v) {
			//assert(!v.HasNaN());
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}

		constexpr Normal3<T> operator-(const Normal3<T> & v)const
		{
			//assert(!v.HasNaN());
			return Normal3<T>(x - v.x, y - v.y, z - v.z);
		}

		constexpr Normal3<T> & operator-=(const Normal3<T> & v)
		{
			//assert(!v.HasNaN());
			x -= v.x;
			y -= v.y;
			z -= v.z;
			return *this;
		}

		constexpr Normal3<T> operator*(const Normal3<T> & v)const
		{
			return { x*v.x,y*v.y,z*v.z };
		}

		constexpr Normal3<T> operator*=(const Normal3<T> & v)
		{
			x *= v.x;
			y *= v.y;
			z *= v.z;
			return *this;
		}

		template<typename U>
		constexpr Normal3<T> operator*(const U & s)const
		{
			//assert(!IsNaN(s));
			return Normal3<T>(s*x, s*y, s*z);
		}
		template<typename U>
		constexpr Normal3<T> & operator*=(const U& s)
		{
			//assert(!IsNaN(s));
			x *= s;
			y *= s;
			z *= s;
			return *this;
		}

		template<typename U>
		constexpr Normal3<T> operator/(U s)const
		{
			//assert(!IsNaN(s));
			const auto inv = static_cast<Float>(1) / s;
			return Normal3<T>(x*inv, y*inv, z*inv);
		}
		template<typename U>
		constexpr Normal3<T> &operator/=(U s)
		{
			//assert(!IsNaN(s));
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

		constexpr Float LengthSquared()const { return x * x + y * y + z * z; }

		constexpr Float Length()const { return std::sqrt(LengthSquared()); }

		constexpr Normal3<T> Normalized()const
		{
			//if len is too small?
			const auto len = Length();
			return (*this) / len;
		}

		constexpr void Normalize()
		{
			//if len is too small?
			const auto len = Length();
			(*this) /= len;
		}

		constexpr bool IsNull()const
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


	//Geometry Inline Functions

	//For Vector3<T>

	template <typename T>
	Vector3<T> Cross(const Vector3<T> &v1, const Vector3<T> &v2) {
		double v1x = v1.x, v1y = v1.y, v1z = v1.z;
		double v2x = v2.x, v2y = v2.y, v2z = v2.z;
		return Vector3<T>((v1y * v2z) - (v1z * v2y), (v1z * v2x) - (v1x * v2z),
			(v1x * v2y) - (v1y * v2x));
	}

	template <typename T>
	Vector3<T> Cross(const Vector3<T> &v1, const Normal3<T> &v2) {
		double v1x = v1.x, v1y = v1.y, v1z = v1.z;
		double v2x = v2.x, v2y = v2.y, v2z = v2.z;
		return Vector3<T>((v1y * v2z) - (v1z * v2y), (v1z * v2x) - (v1x * v2z),
			(v1x * v2y) - (v1y * v2x));
	}

	template <typename T>
	Vector3<T> Cross(const Normal3<T> &v1, const Vector3<T> &v2) {
		double v1x = v1.x, v1y = v1.y, v1z = v1.z;
		double v2x = v2.x, v2y = v2.y, v2z = v2.z;
		return Vector3<T>((v1y * v2z) - (v1z * v2y), (v1z * v2x) - (v1x * v2z),
			(v1x * v2y) - (v1y * v2x));
	}


	template <typename T>
	Vector3<T> Normalize(const Vector3<T> &v) {
		return v / v.Length();
	}

	template<typename T>
	T MinComponent(const Vector3<T> & v)
	{
		return (std::min)(v.x, (std::min)(v.x, v.y));
	}

	template<typename T>
	T MaxComponent(const Vector3<T> & v)
	{
		return (std::max)(v.x, (std::max)(v.x, v.y));
	}

	template<typename T>
	int MaxDimension(const Vector3<T> & v)
	{
		return (v.x > v.y) ? (v.x > v.z ? 0 : 2) : (v.y > v.z ? 1 : 2);
	}

	template<typename T>
	int MinDimension(const Vector3<T> & v)
	{
		return (v.x < v.y) ? (v.x < v.z ? 0 : 2) : (v.y < v.z ? 1 : 2);
	}

	template <typename T>
	Vector3<T> Min(const Vector3<T> &p1, const Vector3<T> &p2) {
		return Vector3<T>(std::min(p1.x, p2.x), std::min(p1.y, p2.y),
			std::min(p1.z, p2.z));
	}

	template <typename T>
	Vector3<T> Max(const Vector3<T> &p1, const Vector3<T> &p2) {
		return Vector3<T>(std::max(p1.x, p2.x), std::max(p1.y, p2.y),
			std::max(p1.z, p2.z));
	}

	template <typename T>
	Vector3<T> Permute(const Vector3<T> &v, int x, int y, int z) {
		return Vector3<T>(v[x], v[y], v[z]);
	}


	// For Point3<T>

	template <typename T>
	Point3<T> Min(const Point3<T> &p1, const Point3<T> &p2) {
		return Point3<T>(std::min(p1.x, p2.x), std::min(p1.y, p2.y),
			std::min(p1.z, p2.z));
	}

	template <typename T>
	Point3<T> Max(const Point3<T> &p1, const Point3<T> &p2) {
		return Point3<T>(std::max(p1.x, p2.x), std::max(p1.y, p2.y),
			std::max(p1.z, p2.z));
	}

	template <typename T>
	Point3<T> Permute(const Point3<T> &p, int x, int y, int z) {
		return Point3<T>(p[x], p[y], p[z]);
	}


	template <typename T>
	Point3<T> Normalize(const Point3<T> &v) {
		return v / v.Length();
	}

	template<typename T>
	T MinComponent(const Point3<T> & v)
	{
		return (std::min)(v.x, (std::min)(v.x, v.y));
	}

	template<typename T>
	T MaxComponent(const Point3<T> & v)
	{
		return (std::max)(v.x, (std::max)(v.x, v.y));
	}

	template<typename T>
	int MaxDimension(const Point3<T> & v)
	{
		return (v.x > v.y) ? (v.x > v.z ? 0 : 2) : (v.y > v.z ? 1 : 2);
	}

	template<typename T>
	int MinDimension(const Point3<T> & v)
	{
		return (v.x < v.y) ? (v.x < v.z ? 0 : 2) : (v.y < v.z ? 1 : 2);
	}



	template<typename T>
	class Bound2
	{
	public:
		Point2<T> min;
		Point2<T> max;
		Bound2():min(std::numeric_limits<T>::max(), std::numeric_limits<T>::max()),max(std::numeric_limits<T>::lowest(), std::numeric_limits<T>::lowest())
		{
		}

		explicit Bound2(const Point2<T> & p):min(p),max(p){}

		Bound2(const Point2<T> & p0,const Point2<T> & p1):min((std::min)(p0.x,p1.x),(std::min)(p0.y,p1.y)),max((std::max)(p0.x, p1.x), (std::max)(p0.y, p1.y)){}

		template<typename U>
		explicit  operator Bound2<U>()const
		{
			return Bound2<U>(U(min), U(max));
		}

		Vector2<T> Diagonal()const
		{
			return max - min;
		}

		const Point2<T> & operator[](int i)const
		{
			assert(i >= 0 && i < 2);
			return *(&min + i);
		}

		Point2<T> & operator[](int i)
		{
			return const_cast<Point2<T>&>(static_cast<const Bound2<T> &>(&this)[i]);
		}

		bool operator==(const Bound2<T> & b)const
		{
			return b.min == min && b.max == max;
		}

		bool operator!=(const Bound2<T> & b)const
		{
			return !(*this == b);
		}

		T Area()const
		{
			const auto d = Diagonal();
			return d.x*d.y;
		}

		int MaximumExtent()const
		{
			const auto d = Diagonal();
			if (d.x > d.y)return 0;
			else return 1;
		}



	};

	class Ray
	{
		///TODO:: Add Medium class pointer later
		//const Medium *medium;

	public:
		Point3f o;
		Vector3f d;
		Float tMax;
		bool negDirection[3];
		Ray(const Vector3f& d, const Point3f& o, Float t = (std::numeric_limits<float>::max)()):o(o), d(d), tMax(t)
		{
			negDirection[0] = d.x < 0;
			negDirection[1] = d.y < 0;
			negDirection[2] = d.z < 0;
		}
		Point3f operator()(float t)const noexcept { return o + t * d; }
		const Point3f & Original()const { return o; }
		const Vector3f & Direction()const { return d; }
		void SetMaxLength(Float t) { tMax = t; }

		//friend class Bound3f;
		friend class Triangle;
		friend class Shape;
		friend class BVHTreeAccelerator;
	};

	/**
	 * \brief A 3-dimension axis-aligned bounding box class
	 * \tparam T 
	 */
	template<typename T>
	class Bound3
	{
	public:

		Point3<T> min;
		Point3<T> max;
	
		constexpr Bound3():min(MAX_Float_VALUE, MAX_Float_VALUE, MAX_Float_VALUE),
			max(LOWEST_Float_VALUE, LOWEST_Float_VALUE, LOWEST_Float_VALUE)
		{
		};

		constexpr Bound3(const Point3<T> & p0, const Point3<T> & p1)noexcept: min((std::min)(p0.x, p1.x), (std::min)(p0.y, p1.y), (std::min)(p0.z, p1.z)),
			max((std::max)(p0.x, p1.x), (std::max)(p0.y, p1.y), (std::max)(p0.z, p1.z))
		{
		}

		explicit Bound3(const Point3<T>& p):min(p.min),max(p.max)
		{
		}

		const Point3f & operator[](int i)const
		{
			assert(i >= 0 && i < 2);
			//if (i == 0)return m_min;
			//if (i == 1)return m_max;
			return *(&min + i);
		}

		Point3<T> & operator[](int i)
		{
			return const_cast<Point3<T>&>(static_cast<const Bound3<T> &>(*this)[i]);
		}

		Point3<T> Corner(int i)const
		{
			return Point3<T>{ (*this)[i & 1].x, (*this)[i & 2?1:0].y, (*this)[i & 4?1:0].z };
		}

		/*
		* Check whether it is intersected with a ray.
		* hit0 stores the nearest ray
		* parameter and hit1 stores the farthest ray parameter
		*/

		/*inline function definitions for AABB*/

		bool Intersect(const Ray & ray, Float * hit0 = nullptr, Float * hit1 = nullptr) const noexcept
		{
			auto t1 = ray.tMax;
			auto t0 = 0.0;
			for (auto i = 0; i < 3; i++)
			{
				const auto inv = 1 / ray.d[i];
				auto tNear = (min[i] - ray.o[i])*inv;
				auto tFar = (max[i] - ray.o[i])*inv;
				if (tNear > tFar) std::swap(tNear, tFar);
				t0 = tNear > t0 ? tNear : t0;
				t1 = tFar < t1 ? tFar : t1;
				if (t0 > t1)return false;
			}
			if (hit0 != nullptr)*hit0 = t0;
			if (hit1 != nullptr)*hit1 = t1;
			return true;
		}

		template<typename U>
		explicit operator Bound3<U>()const
		{
			return Bound3<U>((Point3<U>)min,(Point3<U>)max);
		}

		Point3<T> Center() const
		{
			return (min + max) / 2.0;
		}

		Vector3<T> Diagonal() const
		{
			return max - min;
		}

		T SurfaceArea() const
		{
			const auto d = Diagonal();
			if (d[0] < 0 || d[1] < 0 || d[2] < 0)return Float(0);
			const auto area = (d[0] * d[1] + d[1] * d[2] + d[2] * d[0]) * 2;
			return area;
		}

		T Volume()const
		{
			const auto d = Diagonal();
			return d.x*d.y*d.z;
		}


		int MaximumExtent()const
		{
			const auto d = Diagonal();
			if (d.x > d.y && d.x > d.z)return 0;
			else if(d.y > d.z) return 1;
			else return 2;
		}

		/*
		*Check whether a point is in the bound
		*/

		bool Inside(const Point3<T> & p) const
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

		/*
		* Check whether the bounding box is
		* intersected with another bounding box
		*/

		bool IsIntersectWith(const Bound3<T> & b)const
		{
			return (max.x >= b.min.x && b.max.x >= min.x) &&
				(max.y >= b.min.y && b.max.y >= min.y) &&
				(max.z >= b.min.z && b.max.z >= min.z);
		}

		/*
		* return the common part of two bounding box
		*/

		Bound3<T> IntersectWidth(const Bound3<T> & b) const
		{
			return Bound3<T>{ Point3<T>{(std::max)(min.x, b.min.x),
			(std::max)(min.y, b.min.y),
			(std::max)(min.z, b.min.z)},
			Point3<T>{(std::min)(max.x, b.max.x),
				(std::min)(max.y, b.max.y),
				(std::min)(max.z, b.max.z)}
			}; // For fucking min/max defined in windows.h
		}

		/*
		* Return a minimum bounding box containing the two bounding boxes
		*/
		Bound3<T> UnionWith(const Bound3<T> & b)const
		{
			Bound3<T> a;
			a.min = Point3<T>{
				(std::min)(min.x, b.min.x),
				(std::min)(min.y, b.min.y),
				(std::min)(min.z, b.min.z)
			};
			a.max = Point3<T>{
				(std::max)(max.x, b.max.x),
				(std::max)(max.y, b.max.y),
				(std::max)(max.z, b.max.z)
			}; // For fucking min/max defined in windows.h
			return a;
		}

		/*
		* Return a minimun bounding box containing the
		* bounding box and the point
		*/
		Bound3<T> UnionWith(const Point3<T> & p)const
		{
			Bound3<T> a;
			a.min = Point3<T>(
				(std::min)(min.x, p.x),
				(std::min)(min.y, p.y),
				(std::min)(min.z, p.z)
			);
			a.max = Point3<T>(
				(std::max)(max.x, p.x),
				(std::max)(max.y, p.y),
				(std::max)(max.z, p.z)
			);
			return a;
		}

		friend class BVHTreeAccelerator;
	};

	using Bound3f = Bound3<Float>;
	using Bound3i = Bound3<int>;
	using Bound2f = Bound2<Float>;
	using Bound2i = Bound2<int>;


}

#endif // GEOMETRY_H
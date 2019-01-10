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

	using Vector3f = Vector3<Float>;
	using Vector3i = Vector3<int>;
	using Point3f = Point3<Float>;
	using Point3i = Point3<int>;

	using Vector2f = Vector2<Float>;
	using Vector2i = Vector2<int>;
	using Point2f = Point2<Float>;
	using Point2i = Point2<int>;

	//typedef Vector3<Float> Vector3f;
	//typedef Vector3<int> Vector3i;
	//typedef Point3<Float> Point3f;
	//typedef Point3<int> Point3i;

	//typedef Point2<Float> Point2f;
	//typedef Vector2<Float> Vector2f;
	//typedef Point2<int> Point2i;
	//typedef Vector2<int> Vector2i;

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


}

#endif // GEOMETRY_H
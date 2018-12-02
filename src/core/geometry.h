#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iostream>
#include <cassert>
#include <cmath>
#include <algorithm>


#include "hsl_def.h"

namespace hsl 
{

	template <class T> class Vector2;
	template <class T> class Vector3;
	template <class T> class Point3;
	template <class T> class Point2;

	template<typename T>
	inline bool isNaN(const T t) { return std::isnan(t); }

	template<>
	inline bool isNaN(const int t) { return false; }


	template<typename T>
	class Vector2 {
	public:
		T x, y;
		Vector2() :x(0), y(0) {}
		Vector2(const T & _x, const T & _y) :x(_x), y(_y) {}
		bool HasNaN()const
		{
			return isNaN(x) || isNaN(y);
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
			assert(!isNaN(s));
			const Float inv = static_cast<Float>(1) / s;
			return Vector2<T>(x*inv, y*inv);
		}
		template<typename U>
		Vector2<T> & operator/=(const U & s)
		{
			assert(!isNaN(s));
			const Float inv = static_cast<Float>(1) / s;
			x *= inv; y *= inv;
			return *this;
		}
		template<typename U>
		Vector2<T> operator*(const U & s)
		{
			assert(!isNaN(s));
			return Vector2<T>(s*x, s*y);
		}

		template<typename U>
		Vector2<T> & operator*=(const U & s)
		{
			assert(!isNaN(s));
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

		Float LengthSquared()const { return x * x + y * y; }
		Float Length()const { return std::sqrt(LengthSquared()); }

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


		template<typename X> friend class Point2; //Vector2D can be accessed by all instances of Point2D
	};


	template<typename T>
	inline std::ostream & operator<<(std::ostream & os, const Vector2<T> & v)
	{
		os << "[ " << v.x() << "," << v.y() << "]";
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
			return isNaN(x) || isNaN(y);
		}

		template<typename U>
		explicit operator Vector2<U>()const
		{
			return Vector2<U>(x, y);
		}

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
			assert(!isNaN(s));

			return Point2<T>(s*x, s*y);
		}
		template<typename U>
		Point2<T> & operator*=(const U & s)
		{
			assert(!isNaN(s));
			x *= s; y *= s; return *this;
		}
		template<typename U>
		Point2<T> operator/(const U & s)const
		{
			assert(!isNaN(s));
			const Float inv = static_cast<Float>(1) / s;
			return Point2<T>(x*inv, y*inv);
		}
		template<typename U>
		Point2<T> & operator/=(const U & s)
		{
			assert(!isNaN(s));
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
			return isNaN(x) || isNaN(y) || isNaN(z);
		}
		Vector3<T> operator+(const Vector3<T> & v)const
		{
			assert(!HasNaN(v));
			return Vector3<T>(x + v.x, y + v.y, z + v.z);
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

#ifdef QT_VECTOR3
		operator QVector3D()const
		{
			return QVector3D(xp, yp, zp);
		}
#endif

		//unary operator
		Vector3<T> operator-()const {
			return Vector3<T>(-x, -y, -z);
		}

		template<typename U>
		Vector3<T> operator*(const U & s)const {
			assert(!isNaN(s));
			return Vector3<T>(s*x, s*y, s*z);
		}
		template<typename U>
		Vector3<T> & operator*=(const U& s) {
			assert(!isNaN(s));
			x *= s;
			y *= s;
			z *= s;
			return *this;
		}
		template<typename U>
		Vector3<T> operator/(const U& s)const {
			assert(!isNaN(s));
			const Float inv = static_cast<Float>(1) / s;
			return Vector3<T>(x*inv, y*inv, z*inv);
		}
		template<typename U>
		Vector3<T> & operator/=(const U& s) {
			assert(!isNaN(s));
			const Float inv = static_cast<Float>(1) / s;
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


		template<typename U,typename V>
		static Float Dot(const Vector3<U> & v1,const Vector3<V> & v2)
		{
			return v1.x*v2.x+v1.y*v2.y + v1.z*v2.z;
		}

		template<typename T, typename U, typename V>
		static Vector3<T> Cross(const Vector3<U> & v1, const Vector3<V> & v2)
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
		void Normalize()const
		{
			//if len is too small?
			const auto len = Length();
			(*this) /= len;
		}

		bool isNull()const
		{
			return x == 0 && y == 0 && z == 0;
		}

	};


	template<typename T> 
	std::ostream & operator<<(std::ostream & os, const Vector3<T> & v)
	{
		os << "[" << v.x << "," << v.y << "," << v.z << "]";
		return os;
	}


	template<typename T, typename U> 
	Vector3<T> operator*(const U & s, const Vector3<T> & v)
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
			return isNaN(x) || isNaN(y) || isNaN(z);
		}
		//template<typename U> explicit Point3D(const Point3D<U> & p) :x(T(p.x)), y(T(p.y)), z(T(p.z)) {}

		template<typename U>
		explicit operator Vector3<U>()const
		{
			return Vector3<U>(x, y, z);
		}

#ifdef QT_VECTOR3D
		operator QVector3D()const
		{
			return QVector3D(x, y, z);
		}
#endif



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
			assert(!isNaN(s));
			return Point3<T>(s*x, s*y, s*z);
		}

		Point3<T> operator*(const Point3<T> & p)const
		{
			assert(!isNaN(s));
			return Point3<T>::Dot(*this, p);
		}


		template<typename U>
		Point3<T> & operator*=(const U & s)
		{
			assert(!isNaN(s));
			x *= s;
			y *= s;
			z *= s;
			return *this;
		}

		template<typename U>
		Point3<T> operator/(const U & s)const
		{
			assert(!isNaN(s));
			const Float inv = static_cast<Float>(1) / s;
			return Point3<T>(inv*x, inv*y, inv*z);
		}
		template<typename U>
		Point3<T> & operator/=(const U & s)
		{
			assert(!isNaN(s));
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

		template<typename T>
		Vector3<T> Abs(const Vector3<T> & v) {
			return Vector3<T>(std::abs(v.x()), std::abs(v.y()), std::abs(v.z()));
		}

		
			
	};



	template<typename T> std::ostream & operator<<(std::ostream & os, const Point3<T> & p)
	{
		os << "[" << p.x << "," << p.y << "," << p.z << "]";
		return os;
	}

	template<typename T, typename  U> 
	Point3<T> operator*(const U & s, const Point3<T> & p)
	{
		return p * s;
	}


	typedef Vector3<Float> Vector3f;
	typedef Vector3<int> Vector3i;

	typedef Point3<Float> Point3f;
	typedef Point3<int> Point3i;

	typedef Point2<Float> Point2f;
	typedef Vector2<Float> Vector2f;
	typedef Point2<int> Point2i;
	typedef Vector2<int> Vector2i;


	struct Matrix4x4 {
		// Matrix4x4 Public Methods
		Matrix4x4() {
			m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.f;
			m[0][1] = m[0][2] = m[0][3] = m[1][0] = m[1][2] = m[1][3] = m[2][0] =
				m[2][1] = m[2][3] = m[3][0] = m[3][1] = m[3][2] = 0.f;
		}
		Matrix4x4(Float mat[4][4]);
		Matrix4x4(Float t00, Float t01, Float t02, Float t03, Float t10, Float t11,
			Float t12, Float t13, Float t20, Float t21, Float t22, Float t23,
			Float t30, Float t31, Float t32, Float t33);
		bool operator==(const Matrix4x4 &m2) const {
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					if (m[i][j] != m2.m[i][j]) return false;
			return true;
		}
		bool operator!=(const Matrix4x4 &m2) const {
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					if (m[i][j] != m2.m[i][j]) return true;
			return false;
		}
		friend Matrix4x4 Transpose(const Matrix4x4 &);

		friend Matrix4x4 Inverse(const Matrix4x4 & inv);

		static Matrix4x4 Mul(const Matrix4x4 &m1, const Matrix4x4 &m2) {
			Matrix4x4 r;
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					r.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] +
					m1.m[i][2] * m2.m[2][j] + m1.m[i][3] * m2.m[3][j];
			return r;
		}


		friend std::ostream &operator<<(std::ostream &os, const Matrix4x4 &m) {
			// clang-format off
			os << "[" <<
				m.m[0][0] << "," << m.m[0][1] << "," << m.m[0][2] << "," << m.m[0][3] << "]\n" <<
				m.m[1][0] << "," << m.m[1][1] << "," << m.m[1][2] << "," << m.m[1][3] << "]\n" <<
				m.m[2][0] << "," << m.m[2][1] << "," << m.m[2][2] << "," << m.m[2][3] << "]\n" <<
				m.m[3][0] << "," << m.m[3][1] << "," << m.m[3][2] << "," << m.m[3][3] << "]\n";
			// clang-format on
			return os;
		}

		Float m[4][4];
	};


	class Transform3 {
	public:
		// Transform Public Methods
		Transform3() {}
		Transform3(const Float mat[4][4]) {
			m = Matrix4x4(mat[0][0], mat[0][1], mat[0][2], mat[0][3], mat[1][0],
				mat[1][1], mat[1][2], mat[1][3], mat[2][0], mat[2][1],
				mat[2][2], mat[2][3], mat[3][0], mat[3][1], mat[3][2],
				mat[3][3]);
		}
		Transform3(const Matrix4x4 &m) : m(m) {}

		friend Transform3 Transpose(const Transform3 &t) {
			return Transform3(Transpose(t.m));
		}
		bool operator==(const Transform3 &t) const {
			return t.m == m;
		}
		bool operator!=(const Transform3 &t) const {
			return t.m != m;
		}
		bool IsIdentity() const {
			return (m.m[0][0] == 1.f && m.m[0][1] == 0.f && m.m[0][2] == 0.f &&
				m.m[0][3] == 0.f && m.m[1][0] == 0.f && m.m[1][1] == 1.f &&
				m.m[1][2] == 0.f && m.m[1][3] == 0.f && m.m[2][0] == 0.f &&
				m.m[2][1] == 0.f && m.m[2][2] == 1.f && m.m[2][3] == 0.f &&
				m.m[3][0] == 0.f && m.m[3][1] == 0.f && m.m[3][2] == 0.f &&
				m.m[3][3] == 1.f);
		}
		const Matrix4x4 &Matrix() const { return m; }

		template <typename T>
		inline Point3<T> operator()(const Point3<T> &p) const;

		template <typename T>
		inline Vector3<T> operator()(const Vector3<T> &v) const;

		template<typename T>
		inline Point3<T> operator*(const Point3<T> & p)const;

		template<typename T>
		inline Vector3<T> operator*(const Vector3<T> & v)const;


		friend std::ostream &operator<<(std::ostream &os, const Transform3 &t) 
		{
			os << "t = " << t.m << std::endl;
			return os;
		}

	private:
		Matrix4x4 m;
	};



	template<typename T>
	inline Point3<T> Transform3::operator()(const Point3<T>& p) const
	{
		return (*this)*p;
	}

	template<typename T>
	inline Vector3<T> Transform3::operator()(const Vector3<T>& v) const
	{
		return (*this)*v;
	}

	template<typename T>
	inline Point3<T> Transform3::operator*(const Point3<T>& p) const
	{
		T x = p.x, y = p.y ,z = p.z;
		T xp = m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z + m.m[0][3];
		T yp = m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z + m.m[1][3];
		T zp = m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z + m.m[2][3];
		T wp = m.m[3][0] * x + m.m[3][1] * y + m.m[3][2] * z + m.m[3][3];

		if (wp == 1)
			return Point3<T>(xp, yp, zp);
		else
			return Point3<T>(xp, yp, zp) / wp;


	}

	template<typename T>
	inline Vector3<T> Transform3::operator*(const Vector3<T>& v) const
	{
		T x = v.x,y = v.y, z = v.z;
		return Vector3<T>(m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z,
			m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z,
			m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z);
	}


	// Ray
	class Ray
	{

		Point3f m_o;
		Vector3f m_d;
		Float m_tMax;
		bool m_negDirection[3];

	public:
		Ray(const Vector3f & d, const Point3f & o, Float t = MAX_Float_VALUE)noexcept:m_o(o),m_d(d), m_tMax(t)
		{
			m_negDirection[0] = d.x < 0;
			m_negDirection[1] = d.y < 0;
			m_negDirection[2] = d.z < 0;
		}
		Point3f operator()(float t)const noexcept { return m_o + t * m_d; }
		const Point3f & original()const {return m_o;}
		const Vector3f & direction()const {return m_d;}
		void setMaxLength(Float t) {m_tMax = t;}

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
		AABB() :m_min(MAX_Float_VALUE, MAX_Float_VALUE, MAX_Float_VALUE), m_max(LOWEST_Float_VALUE, LOWEST_Float_VALUE, LOWEST_Float_VALUE)
		{
			//construct a empty bounding box
		}

		AABB(const Point3f & p0, const Point3f & p1)noexcept :
			m_min((std::min)(p0.x, p1.x), (std::min)(p0.y, p1.y), (std::min)(p0.z, p1.z)),
			m_max((std::max)(p0.x, p1.x), (std::max)(p0.y, p1.y), (std::max)(p0.z, p1.z))		// For fucking min/max defined in windows.h
		{}

		AABB(const Point3f & p) :m_min(p), m_max(p) {}

		const Point3f & operator[](int i)const 
		{
			assert(i >= 0 && i < 2);
			if (i == 0)return m_min;
			if (i == 1)return m_max;
			return Point3f{};
		}

		Point3f & operator[](int i)noexcept 
		{
			return const_cast<Point3f&>(static_cast<const AABB &>(*this)[i]);
		}

		Point3f corner(int i)const 
		{
			return Point3f{(*this)[i & 1].x, (*this)[i & 2].y, (*this)[i & 4].z};
		}
		/*
		* Check whether it is intersected with a ray.
		* hit0 stores the nearest ray
		* parameter and hit1 stores the farthest ray parameter
		*/

		/*inline function definitions for AABB*/

		bool intersect(const Ray & ray, Float * hit0 = nullptr, Float * hit1 = nullptr) const noexcept;

		Point3f center()const 
		{
			return (m_min + m_max) / 2;
		}

		Vector3f diagnal()const
		{
			return m_max - m_min;
		}

		Float surfaceArea() const;

		/*
		*Check whether a point is in the bound
		*/

		bool inside(const Point3f & p) const
		{
			return (p.x >= m_min.x && p.y <= m_max.x &&
				p.y >= m_min.y && p.y <= m_max.y &&
				p.z >= m_min.z && p.z <= m_max.z);
		}

		bool insideEx(const Point3f & p) const
		{
			return (p.x >= m_min.x && p.y < m_max.x &&
				p.y >= m_min.y && p.y < m_max.y &&
				p.z >= m_min.z && p.z < m_max.z);
		}

		/*
		* Check whether the bounding box is
		* intersected with another bounding box
		*/

		bool isIntersectWith(const AABB & b)const;
		/*
		* return the common part of two bounding box
		*/
		AABB intersectWith(const AABB & b) const;

		/*
		* Return a minimum bounding box containing the two bounding boxes
		*/
		AABB unionWith(const AABB & b)const;
		/*
		* Return a minimun bounding box containing the
		* bounding box and the point
		*/
		AABB unionWith(const Point3f & p)const;


		friend class BVHTreeAccelerator;
	};

	inline
	Float AABB::surfaceArea() const
	{
		const auto d = diagnal();
		if (d[0] < 0 || d[1] < 0 || d[2] < 0)return Float(0);
		const auto area = (d[0] * d[1] + d[1] * d[2] + d[2] * d[0]) * 2;
		return area;
	}

	inline
	bool AABB::isIntersectWith(const AABB & b)const
	{
		return (m_max.x >= b.m_min.x && b.m_max.x >= m_min.x) &&
			(m_max.y >= b.m_min.y && b.m_max.y >= m_min.y) &&
			(m_max.z >= b.m_min.z && b.m_max.z >= m_min.z);
	}

	inline
		AABB AABB::unionWith(const Point3f & p)const {
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

	inline
	AABB AABB::unionWith(const AABB & b) const
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
	inline
		AABB AABB::intersectWith(const AABB & b) const
	{
		return AABB{Point3f{(std::max)(m_min.x, b.m_min.x),
			(std::max)(m_min.y, b.m_min.y),
			(std::max)(m_min.z, b.m_min.z)},
			Point3f{(std::min)(m_max.x, b.m_max.x),
				(std::min)(m_max.y, b.m_max.y),
				(std::min)(m_max.z, b.m_max.z)}
			}; // For fucking min/max defined in windows.h
	}


}
#endif // GEOMETRY_H
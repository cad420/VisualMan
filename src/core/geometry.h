#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iostream>
#include <cassert>
#include <cmath>

#include "hsl_def.h"

namespace hsl {

	template <class T> class Vector2;
	template <class T> class Vector3;
	template <class T> class Point3;
	template <class T> class Point2;

	template<typename T>
	inline bool isNaN(const T  t) { return std::isnan(t); }

	template<>
	inline bool isNaN(const int t) { return false; }


	template<typename T>
	class Vector2 {
		T xp, yp;
	public:
		Vector2() :xp(0), yp(0) {}
		Vector2(const T & _x, const T & _y) :xp(_x), yp(_y) {}
		//explicit Vector2D(const Vector2D<T> & v) :xp(v.xp), yp(v.yp) {}
		bool HasNaN()const
		{
			return isNaN(xp) || isNaN(yp);
		}

		Vector2<T> operator+(const Vector2<T> & p)const
		{
			assert(!p.HasNaN());
			return Vector2<T>(xp + p.xp, yp + p.yp);
		}

		Point2<T> operator+(const Point2<T> & v)const
		{
			assert(!v.HasNaN());
			return Point2<T>(xp + v.xp, yp + v.yp);
		}

		Vector2<T> & operator+=(const Vector2<T> & v) {
			assert(!v.HasNaN());
			xp += v.xp;
			yp += v.yp;
			return *this;
		}

		Vector2<T> operator-(const Vector2<T> & v)
		{
			assert(!v.HasNaN());
			return Vector2<T>(xp - v.xp, yp - v.yp);
		}

		Vector2<T> & operator-=(const Vector2<T> & v)
		{
			assert(!v.HasNaN());
			xp -= v.xp;
			yp -= v.yp;
			return *this;
		}
		template<typename U>
		Vector2<T> operator/(const U & s)const
		{
			assert(!isNaN(s));
			const Float inv = static_cast<Float>(1) / s;
			return Vector2<T>(xp*inv, yp*inv);
		}
		template<typename U>
		Vector2<T> & operator/=(const U & s)
		{
			assert(!isNaN(s));
			const Float inv = static_cast<Float>(1) / s;
			xp *= inv; yp *= inv;
			return *this;
		}
		template<typename U>
		Vector2<T> operator*(const U & s)
		{
			assert(!isNaN(s));
			return Vector2<T>(s*xp, s*yp);
		}

		template<typename U>
		Vector2<T> & operator*=(const U & s)
		{
			assert(!isNaN(s));
			xp *= s;
			yp *= s;
			return *this;
		}



		Vector2<T> operator-()
		{
			return Vector2<T>(-xp, -yp);
		}

		const T & operator[](int i)const
		{
			assert(i >= 0 && i < 2); return *(&xp + i);
		}

		T & operator[](int i)
		{
			assert(i >= 0 && i < 2); return *(&xp + i);
		}


		bool operator==(const Vector2<T> & v)
		{
			return xp == v.xp&&yp == v.yp;
		}
		bool operator!=(const Vector2<T> & v)
		{
			return !(*this == v);
		}

		Float LengthSquared()const { return xp * xp + yp * yp; }
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


		const T & x()const
		{
			return xp;
		}
		T& x()
		{
			return xp;
		}

		const T & y()const
		{
			return yp;
		}
		T& y()
		{
			return yp;
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
		T xp, yp;
	public:
		Point2() :xp(0), yp(0) {}
		Point2(const T & x, const T & y) :xp(x), yp(y)
		{
			assert(!HasNaN());
		}
		//  explicit Point2D(const Point2D<T> & v) :xp(v.xp), yp(v.yp)
		//  {
			  //assert(!HasNaN());
		//  }

		bool HasNaN()const
		{
			return isNaN(xp) || isNaN(yp);
		}

		template<typename U>
		explicit operator Vector2<U>()const
		{
			return Vector2<U>(xp, yp);
		}

		Point2<T> operator+(const Vector2<T> & v)
		{
			return Point2<T>(xp + v.xp, yp + v.yp);
		}
		Point2<T> operator+(const Point2<T> & p)
		{
			return Point2<T>(xp + p.xp, yp + p.yp);
		}
		Point2<T> operator+=(const Point2<T> & p)
		{
			return Point2<T>(xp + p.xp, yp + p.yp);
		}
		Point2<T> & operator+=(const Vector2<T> & v)
		{
			xp += v.xp;
			yp += v.yp;
			return *this;
		}

		Point2<T> operator-(const Vector2<T> & v)
		{
			assert(!v.HasNaN());
			return Point2<T>(xp - v.xp, yp - v.yp);
		}
		Vector2<T> operator-(const Point2<T> & p)
		{
			assert(!p.HasNaN());
			return Vector2<T>(xp - p.xp, yp - p.yp);
		}
		Point2<T> operator-()
		{
			return Point2<T>(-xp, -yp);
		}

		Point2<T> & operator-=(const Vector2<T> & v)
		{
			assert(!v.HasNaN());
			xp -= v.xp; yp -= v.yp; return *this;
		}
		template<typename U>
		Point2<T> operator*(const U & s)
		{
			assert(!isNaN(s));

			return Point2<T>(s*xp, s*yp);
		}
		template<typename U>
		Point2<T> & operator*=(const U & s)
		{
			assert(!isNaN(s));
			xp *= s; yp *= s; return *this;
		}
		template<typename U>
		Point2<T> operator/(const U & s)const
		{
			assert(!isNaN(s));
			const Float inv = static_cast<Float>(1) / s;
			return Point2<T>(xp*inv, yp*inv);
		}
		template<typename U>
		Point2<T> & operator/=(const U & s)
		{
			assert(!isNaN(s));
			const Float inv = static_cast<Float>(1) / s;
			xp *= inv; yp *= inv;
			return *this;
		}
		const T & operator[](int i)const
		{
			assert(i >= 0 && i < 2);
			return *(&xp + i);
		}
		T & operator[](int i)
		{
			assert(i >= 0 && i < 2);
			return *(&xp + i);
		}

		bool operator==(const Point2<T> & p)
		{
			return xp == p.xp&&yp == p.yp;
		}
		bool operator!=(const Point2<T> & p)
		{
			return !(*this == p);
		}

		T & x()
		{
			return xp;
		}
		const T& x()const
		{
			return xp;
		}
		T & y()
		{
			return yp;
		}
		const T & y()const
		{
			return yp;
		}

	};

	template<typename T> std::ostream & operator<<(std::ostream & os, const Point2<T> & p)
	{
		os << "[" << p.x() << "," << p.y() << "]";
		return os;
	}

	template<typename T, typename U> inline Point2<T> operator*(const U & s, const Point2<T> & v)
	{
		return Point2<T>(s*v.x(), s*v.y());
	}
	template<typename T>
	class Vector3 {
		T xp, yp, zp;
	public:
		Vector3() :xp(0), yp(0), zp(0) {}
		Vector3(const T &x, const T &y, const T& z) :xp(x), yp(y), zp(z)
		{
			assert(!HasNaN());
		}
		//Vector3D(const Vector3D<T> & v) :x(T(v.x)), y(T(v.y)), z(T(v.z)) {}
		bool HasNaN()const
		{
			return isNaN(xp) || isNaN(yp) || isNaN(zp);
		}
		Vector3<T> operator+(const Vector3<T> & v)const
		{
			assert(!HasNaN(v));
			return Vector3<T>(xp + v.xp, yp + v.yp, zp + v.zp);
		}

		Vector3<T> & operator+=(const Vector3<T> & v) {
			assert(!v.HasNaN());
			xp += v.xp;
			yp += v.yp;
			zp += v.zp;
			return *this;
		}
		Vector3<T> operator-(const Vector3<T> & v)const
		{
			assert(!v.HasNaN());
			return Vector3<T>(xp - v.xp, yp - v.yp, zp - v.zp);
		}

		Vector3<T> & operator-=(const Vector3<T> & v)
		{
			assert(!v.HasNaN());
			xp -= v.xp;
			yp -= v.yp;
			zp -= v.zp;
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
			return Vector3<T>(-xp, -yp, -zp);
		}

		template<typename U>
		Vector3<T> operator*(const U & s)const {
			assert(!isNaN(s));
			return Vector3<T>(s*xp, s*yp, s*zp);
		}
		template<typename U>
		Vector3<T> & operator*=(const U& s) {
			assert(!isNaN(s));
			xp *= s;
			yp *= s;
			zp *= s;
			return *this;
		}
		template<typename U>
		Vector3<T> operator/(const U& s)const {
			assert(!isNaN(s));
			const Float inv = static_cast<Float>(1) / s;
			return Vector3<T>(xp*inv, yp*inv, zp*inv);
		}
		template<typename U>
		Vector3<T> & operator/=(const U& s) {
			assert(!isNaN(s));
			const Float inv = static_cast<Float>(1) / s;
			xp *= inv; yp *= inv; zp *= inv;
			return *this;
		}

		const T & operator[](int i)const {
			assert(i >= 0 && i < 3);
			return *(&xp + i);
		}
		T & operator[](int i) {
			assert(i >= 0 && i < 3);
			return *(&xp + i);
		}
		const T & x()const { return xp; }
		T & x() { return xp; }
		const T & y()const { return yp; }
		T & y() { return yp; }
		const T & z()const { return zp; }
		T & z() { return zp; }

		template<typename T,typename U,typename V>
		static Vector3<T> Dot(const Vector3<U> & v1,const Vector3<V> & v2)
		   {
			return Vector3<T>(v1.xp*v2.xp,v1.yp*v2.yp,v1.zp*v2.zp);
		   }

		template<typename T, typename U, typename V>
		static Vector3<T> Cross(const Vector3<U> & v1, const Vector3<V> & v2)
		{
			return Vector3<T>(
				v1.yp * v2.zp - v1.zp * v2.yp,
				v1.zp * v2.xp - v1.xp * v2.zp,
				v1.xp * v2.yp - v1.yp * v2.xp);
		}

		Float LengthSquared()const { return xp * xp + yp * yp + zp * zp; }

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
			return xp == 0 && yp == 0 && zp == 0;
		}

	};


	template<typename T> 
	std::ostream & operator<<(std::ostream & os, const Vector3<T> & v)
	{
		os << "[" << v.x() << "," << v.y() << "," << v.z() << "]";
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
		T xp, yp, zp;
	public:
		Point3() :xp(0), yp(0), zp(0) {}
		Point3(const T &x, const T &y, const T& z) :xp(x), yp(y), zp(z)
		{
			assert(!HasNaN());
		}

		bool HasNaN()const
		{
			return isNaN(xp) || isNaN(yp) || isNaN(zp);
		}
		//template<typename U> explicit Point3D(const Point3D<U> & p) :x(T(p.x)), y(T(p.y)), z(T(p.z)) {}

		template<typename U>
		explicit operator Vector3<U>()const
		{
			return Vector3<U>(xp, yp, zp);
		}

#ifdef QT_VECTOR3D
		operator QVector3D()const
		{
			return QVector3D(xp, yp, zp);
		}
#endif


		Point3<T> operator+(const Point3<T> & p)const
		{
			assert(!p.HasNaN());
			return Point3<T>(xp + p.xp, yp + p.yp, zp + p.zp);
		}

		Point3<T> operator+(const Vector3<T> & v)const
		{
			assert(!v.HasNaN());
			return Point3<T>(xp + v.xp, yp + v.yp, zp + v.zp);
		}

		Point3<T> & operator+=(const Vector3<T> & v)
		{
			assert(!v.HasNaN());
			xp += v.xp;
			yp += v.yp;
			zp += v.zp;
			return *this;
		}

		Point3<T> & operator+=(const Point3<T> & p)
		{
			assert(!p.HasNaN());
			xp += p.xp;
			yp += p.yp;
			zp += p.zp;
			return *this;
		}

		template<typename U>
		Point3<T> operator*(const U & s)const
		{
			assert(!isNaN(s));
			return Point3<T>(s*xp, s*yp, s*zp);
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
			xp *= s;
			yp *= s;
			zp *= s;
			return *this;
		}

		template<typename U>
		Point3<T> operator/(const U & s)const
		{
			assert(!isNaN(s));
			const Float inv = static_cast<Float>(1) / s;
			return Point3<T>(inv*xp, inv*yp, inv*zp);
		}
		template<typename U>
		Point3<T> & operator/=(const U & s)
		{
			assert(!isNaN(s));
			const Float inv = static_cast<Float>(1) / s;
			xp *= inv;
			yp *= inv;
			zp *= inv;
			return *this;
		}


		Vector3<T> operator-(const Point3<T> & p)const
		{
			assert(!p.HasNaN());
			return Vector3<T>(xp - p.xp, yp - p.yp, zp - p.zp);
		}
		Point3<T> operator-(const Vector3<T> & v) const
		{
			assert(!v.HasNaN());
			return Point3<T>(xp - v.xp, yp - v.yp, zp - v.zp);
		}
		Point3<T> & operator-=(const Vector3<T> & v)
		{
			assert(!v.HasNaN());
			xp -= v.xp;
			yp -= v.yp;
			zp -= v.zp;
			return *this;
		}
		const T& operator[](int i)const
		{
			assert(i >= 0 && i < 3);
			return *(&xp + i);
		}
		T & operator[](int i)
		{
			assert(i >= 0 && i < 3);
			return *(&xp + i);
		}
		const T & x()const
		{
			return xp;
		}
		T & x()
		{
			return xp;
		}

		const T & y()const
		{
			return yp;
		}

		T & y()
		{
			return yp;
		}

		const T & z()const
		{
			return zp;
		}

		T & z()
		{
			return zp;
		}

		bool isNull()const
		{
			return xp == 0 && yp == 0 && zp == 0;
		}

		template<typename T>
		static Float Dot(const Point3<T> & v1, const Point3<T> & v2)
		{
			return v1.xp*v2.xp, v1.yp*v2.yp, v1.zp*v2.zp;
		}

		template<typename T>
		static Point3<T> Cross(const Point3<T> & v1, const Point3<T> & v2)
		{
			return Point3<T>(
				v1.yp * v2.zp - v1.zp * v2.yp,
				v1.zp * v2.xp - v1.xp * v2.zp,
				v1.xp * v2.yp - v1.yp * v2.xp);
		}

		template<typename T>
		Vector3<T> Abs(const Vector3<T> & v) {
			return Vector3<T>(std::abs(v.x()), std::abs(v.y()), std::abs(v.z()));
		}
			
	};

	template<typename T> std::ostream & operator<<(std::ostream & os, const Point3<T> & p)
	{
		os << "[" << p.x() << "," << p.y() << "," << p.z() << "]";
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


		friend std::ostream &operator<<(std::ostream &os, const Transform3 &t) {
			os << "t=" << t.m << std::endl;
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
		T x = p.x(), y = p.y(), z = p.z();
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
		T x = v.x(), y = v.y(), z = v.z();
		return Vector3<T>(m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z,
			m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z,
			m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z);
	}

}
#endif // GEOMETRY_H
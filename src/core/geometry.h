#ifndef GEOMETRY_H
#define GEOMETRY_H

//#include "core.h"
#include <cassert>
//#include "core.h"

#include "utils.h"


#define QT_VECTOR3



//
//constexpr Float PI = 3.1415926535;
//constexpr Float LOWEST_Float_VALUE = std::numeric_limits<Float>::lowest();
//constexpr Float MAX_Float_VALUE = std::numeric_limits<Float>::max();

enum class State {
	Uninitialized
};

template<typename T> class Point2D;
template<typename T> class Point3D;
template<typename T> class Vector2D;
template<typename T> class Vector3D;

template<typename T>
class Vector2D {
	T xp, yp;
public:
	Vector2D():xp(0),yp(0){}
    Vector2D(const T & x , const T & y ) :xp(x), yp(y) {}
    //explicit Vector2D(const Vector2D<T> & v) :xp(v.xp), yp(v.yp) {}
	bool HasNaN()const 
	{
		return isNaN(xp) || isNaN(yp);
	}
	
    Vector2D<T> operator+(const Vector2D<T> & p)const 
	{
		assert(!v.HasNaN());
        return Vector2D<T>(xp + p.xp, yp + p.yp);
    }

	Point2D<T> operator+(const Point2D<T> & v)const
    {
		assert(!v.HasNaN());
		return Point2D<T>(xp + v.xp, yp + v.yp);
    }

    Vector2D<T> & operator+=(const Vector2D<T> & v) {
		assert(!v.HasNaN());
        xp += v.xp;
        yp += v.yp;
        return *this;
    }

    Vector2D<T> operator-(const Vector2D<T> & v) 
	{
		assert(!v.HasNaN());
        return Vector2D<T>(xp - v.xp, yp - v.yp);
    }

    Vector2D<T> & operator-=(const Vector2D<T> & v) 
	{
		assert(!v.HasNaN());
        xp -= v.xp;
        yp -= v.yp;
        return *this;
    }
	template<typename U>
	Vector2D<T> operator/(const U & s)const
    {
		assert(!isNaN(s));
		const Float inv = static_cast<Float>(1) / s; 
    	return Vector2D<T>(xp*inv, yp*inv);
    }
	template<typename U>
	Vector2D<T> & operator/=(const U & s)
    {
		assert(!isNaN(s));
		const Float inv = static_cast<Float>(1) / s; 
    	xp *= inv; yp *= inv;
    	return *this;
    }
	template<typename U>
    Vector2D<T> operator*(const U & s) 
	{
		assert(!isNaN(s));
        return Vector2D<T>(s*xp, s*yp);
    }

	template<typename U>
    Vector2D<T> & operator*=(const U & s) 
	{
		assert(!isNaN(s));
        xp *= s;
    	yp *= s;
    	return *this;
    }



    Vector2D<T> operator-() 
	{
        return Vector2D<T>(-xp, -yp);
    }

	const T & operator[](int i)const 
	{
		assert(i >= 0 && i < 2);return *(&xp + i);
	}

	T & operator[](int i) 
	{
		assert(i >= 0 && i < 2);return *(&xp + i);
	}


	bool operator==(const QVector<T> & v)
    {
		return xp == v.xp&&yp == v.yp;
    }
	bool operator!=(const QVector<T> & v)
    {
		return !(*this == v);
    }

	Float LengthSquared()const { return xp * xp + yp * yp; }
	Float Length()const { return std::sqrt(LengthSquared()); }

	Vector2D<T> normalized()const
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

    template<typename X> friend class Point2D; //Vector2D can be accessed by all instances of Point2D



};


template<typename T>
inline std::ostream & operator<<(std::ostream & os,const Vector2D<T> & v)
{
	os << "[ " << v.x() << "," << v.y()<< "]";
	return os;
}


/*
* Point2D
*/
template<typename T>
class Point2D {
	T xp, yp;
public:		
	Point2D():xp(0),yp(0){}
    Point2D(const T & x, const T & y ) :xp(x), yp(y)
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
	explicit operator Vector2D<U>()const
	{
		return Vector2D<U>(xp, yp);
	}

    Point2D<T> operator+(const Vector2D<T> & v) 
	{
        return Point2D<T>(xp + v.xp, yp + v.yp);
    }
    Point2D<T> operator+(const Point2D<T> & p) 
    {
        return Point2D<T>(xp + p.xp, yp + p.yp);
    }
	Point2D<T> operator+=(const Point2D<T> & p)
	{
		return Point2D<T>(xp + p.xp, yp + p.yp);
	}
    Point2D<T> & operator+=(const Vector2D<T> & v) 
	{
        xp += v.xp;
        yp += v.yp;
        return *this;
    }

    Point2D<T> operator-(const Vector2D<T> & v) 
	{
		assert(!v.HasNaN());
        return Point2D<T>(xp - v.xp, yp - v.yp);
    }
	Vector2D<T> operator-(const Point2D<T> & p)
	{
		assert(!p.HasNaN());
		return Vector2D<T>(xp - p.xp, yp - p.yp);
    }
	Point2D<T> operator-()
	{
		return Point2D<T>(-xp, -yp);
	}

    Point2D<T> & operator-=(const Vector2D<T> & v)
	{
		assert(!v.HasNaN());
        xp -= v.xp;yp -= v.yp;return *this;
    }
	template<typename U>
    Point2D<T> operator*(const U & s) 
	{
		assert(!isNaN(s));

        return Point2D<T>(s*xp, s*yp);
    }
	template<typename U>
    Point2D<T> & operator*=(const U & s) 
	{
		assert(!isNaN(s));
        xp *= s;yp *= s;return *this;
    }
	template<typename U>
	Point2D<T> operator/(const U & s)const
	{
		assert(!isNaN(s));
		const Float inv = static_cast<Float>(1) / s;
		return Point2D<T>(xp*inv, yp*inv);
	}
	template<typename U>
	Point2D<T> & operator/=(const U & s)
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

	bool operator==(const Point2D<T> & p)
    {
		return xp == p.xp&&yp == p.yp;
    }
	bool operator!=(const Point2D<T> & p)
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

template<typename T> std::ostream & operator<<(std::ostream & os,const Point2D<T> & p)
{
	os << "[" << p.x() << "," << p.y() << "]";
	return os;
}

template<typename T,typename U> inline Point2D<T> operator*(const U & s,const Point2D<T> & v)
{
	return Point2D<T>(s*v.x(), s*v.y());
}


//class Normal2f {
//public:
//	Float x;
//	Float y;
//	explicit Normal2f() = default;
//	Normal2f(Float xp, Float yp) :x(xp), y(yp) {
//		
//	}
//	bool nan()const {
//		return isNaN(x) || isNaN(y);
//	}
//	//Float x()const { return x; }
//	//Float y()const { return y; }
//	Normal2f operator+(const Normal2f & norm)const { return Normal2f(x + norm.x, y + norm.y); }
//	Normal2f &operator += (const Normal2f & norm) { x += norm.x; y += norm.y; return *this; }
//
//	Normal2f operator-(const Normal2f & norm)const { return Normal2f(x - norm.x, y - norm.y); }
//	Normal2f &operator-=(const Normal2f & norm) { x -= norm.x; y -= norm.y; return *this; }
//
//	template<typename T> Normal2f operator*(const T & s) { return Normal2f(x*s, y*s); }
//
//
//	Float dotProduct(const Normal2f & norm)const { return x * norm.x + y * norm.y; }
//
//};
//template<typename T>
//inline
//Normal2f operator*(const T & s, const Normal2f & norm) {
//	return norm * s;
//}




/*
* Vector3D
*/
template<typename T>
class Vector3D {
	T xp, yp, zp;
public:
    Vector3D():xp(0),yp(0),zp(0){}
    Vector3D(const T &x , const T &y, const T& z ) :xp(x), yp(y), zp(z)
    {
		assert(!HasNaN());
    }
    //Vector3D(const Vector3D<T> & v) :x(T(v.x)), y(T(v.y)), z(T(v.z)) {}
	bool HasNaN()const
    {
		return isNaN(xp) || isNaN(yp) || isNaN(zp);
    }
    Vector3D<T> operator+(const Vector3D<T> & v)const 
	{
		assert(!HasNaN(v));
        return Vector3D<T>(xp + v.xp, yp + v.yp, zp + v.zp);
    }

    Vector3D<T> & operator+=(const Vector3D<T> & v) {
		assert(!v.HasNaN());
        xp += v.xp;
        yp += v.yp;
        zp += v.zp;
        return *this;
    }
	Vector3D<T> operator-(const Vector3D<T> & v)const
    {
		assert(!v.HasNaN());
		return Vector3D<T>(xp - v.xp, yp - v.yp, zp - v.zp);
    }

	Vector3D<T> & operator-=(const Vector3D<T> & v)
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
	Vector3D<T> operator-()const {
		return Vector3D<T>(-xp, -yp, -zp);
	}

	template<typename U>
    Vector3D<T> operator*(const U & s)const {
		assert(!isNaN(s));
        return Vector3D<T>(s*xp, s*yp, s*zp);
    }
	template<typename U>
    Vector3D<T> & operator*=(const U& s) {
		assert(!isNaN(s));
        xp *= s;
        yp *= s;
        zp *= s;
        return *this;
    }
	template<typename U>
    Vector3D<T> operator/(const U& s)const {
		assert(!isNaN(s));
        const Float inv = static_cast<Float>(1) / s;
        return Vector3D<T>(xp*inv, yp*inv, zp*inv);
    }
	template<typename U>
    Vector3D<T> & operator/=(const U& s) {
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

	//template<typename T,typename U,typename V>
	//static Vector3D<T> dotProduct(const Vector3D<U> & v1,const Vector3D<V> & v2)
    //   {
	//	return Vector3D<T>(v1.xp*v2.xp,v1.yp*v2.yp,v1.zp*v2.zp);
    //   }

	//template<typename T, typename U, typename V>
	//static Vector3D<T> crossProduct(const Vector3D<U> & v1, const Vector3D<V> & v2)
	//{
	//	return Vector3D<T>(
	//		v1.yp * v2.zp - v1.zp * v2.yp,
	//		v1.zp * v2.xp - v1.xp * v2.zp,
	//		v1.xp * v2.yp - v1.yp * v2.xp);
	//}

	Float LengthSquared()const { return xp * xp + yp * yp + zp*zp; }
	
	Float lengthSquared()const { return xp * xp + yp * yp + zp*zp; }//deprecated

	Float Length()const { return std::sqrt(LengthSquared()); }
	
	Float length()const { return std::sqrt(LengthSquared()); }//deprecated

	Vector3D<T> normalized()const
    {
		//if len is too small?
		const Float len = Length();
		return (*this) / len;
    }
	void normalize()const
	{
		//if len is too small?
		const Float len = Length();
		(*this) /= len;
	}

	template<typename T>
	static Float Dot(const Vector3D<T> & v1, const Vector3D<T> & v2)
	{
		return v1.xp*v2.xp, v1.yp*v2.yp, v1.zp*v2.zp;
	}

	template<typename T>
	static Float dotProduct(const Vector3D<T> & v1, const Vector3D<T> & v2)
	{
		return Dot(v1,v2);
	}


	template<typename T>
	static Vector3D<T> Cross(const Vector3D<T> & v1, const Vector3D<T> & v2)
	{
		return Vector3D<T>(
			v1.yp * v2.zp - v1.zp * v2.yp,
			v1.zp * v2.xp - v1.xp * v2.zp,
			v1.xp * v2.yp - v1.yp * v2.xp);
	}

	template<typename T>
	static Vector3D<T> crossProduct(const Vector3D<T> & v1, const Vector3D<T> & v2)
	{
		return Cross(v1, v2);
	}

	bool isNull()const
    {
		return xp == 0 && yp == 0 && zp == 0;
    }

};



template<typename T> std::ostream & operator<<(std::ostream & os,const Vector3D<T> & v)
{
	os << "[" << v.x() << "," << v.y() << "," << v.z() << "]";
	return os;
}


template<typename T,typename U> inline Vector3D<T> operator*(const U & s, const Vector3D<T> & v) 
{
    return v * s;
}

template<typename T> inline Vector3D<T> Abs(const Vector3D<T> & v) {
    return Vector3D<T>(std::abs(v.x()), std::abs(v.y()), std::abs(v.z()));
}


/*
* Point3D
*/

template<typename T>
class Point3D {
	T xp, yp, zp;
public:
    Point3D():xp(0),yp(0),zp(0){}
    Point3D(const T &x , const T &y , const T& z) :xp(x), yp(y), zp(z)
    {
		assert(!HasNaN());
    }

	bool HasNaN()const
    {
		return isNaN(xp) || isNaN(yp) || isNaN(zp);
    }
    //template<typename U> explicit Point3D(const Point3D<U> & p) :x(T(p.x)), y(T(p.y)), z(T(p.z)) {}

	template<typename U>
	explicit operator Vector3D<U>()const
	{
		return Vector3D<U>(xp, yp, zp);
	}

#ifdef QT_VECTOR3
	operator QVector3D()const
	{
		return QVector3D(xp, yp, zp);
	}
#endif


	Point3D<T> operator+(const Point3D<T> & p)const
	{
		assert(!p.HasNaN());
		return Point3D<T>(xp + p.xp, yp + p.yp, zp + p.zp);
	}

    Point3D<T> operator+(const Vector3D<T> & v)const
    {
		assert(!v.HasNaN());
	    return Point3D<T>(xp + v.xp, yp + v.yp, zp + v.zp);
    }

    Point3D<T> & operator+=(const Vector3D<T> & v) 
	{
		assert(!v.HasNaN());
        xp += v.xp;
        yp += v.yp;
        zp += v.zp;
        return *this;
    }

    Point3D<T> & operator+=(const Point3D<T> & p) 
	{
		assert(!p.HasNaN());
        xp += p.xp;
        yp += p.yp;
        zp += p.zp;
        return *this;
    }

	template<typename U>
    Point3D<T> operator*(const U & s)const
    {
		assert(!isNaN(s));
        return Point3D<T>(s*xp, s*yp, s*zp);
    }

	Point3D<T> operator*(const Point3D<T> & p)const
    {
		assert(!isNaN(s));
		return Point3D<T>::dotProduct(*this, p);
    }


	template<typename U>
    Point3D<T> & operator*=(const U & s)
    {
		assert(!isNaN(s));
        xp *= s;
        yp *= s;
        zp *= s;
        return *this;
    }

	template<typename U>
	Point3D<T> operator/(const U & s)const
	{
		assert(!isNaN(s));
		const Float inv = static_cast<Float>(1) / s;
		return Point3D<T>(inv*xp, inv*yp, inv*zp);
	}
	template<typename U>
	Point3D<T> & operator/=(const U & s)
	{
		assert(!isNaN(s));
		const Float inv = static_cast<Float>(1) / s;
		xp *= inv;
		yp *= inv;
		zp *= inv;
		return *this;
	}


    Vector3D<T> operator-(const Point3D<T> & p)const 
	{
		assert(!p.HasNaN());
        return Vector3D<T>(xp - p.xp, yp - p.yp, zp - p.zp);
    }
    Point3D<T> operator-(const Vector3D<T> & v) const
	{
		assert(!v.HasNaN());
        return Point3D<T>(xp - v.xp, yp - v.yp, zp - v.zp);
    }
    Point3D<T> & operator-=(const Vector3D<T> & v)
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
	T &x()
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

	//template<typename T>
	//static Float dotProduct(const Point3D<T> & v1, const Point3D<T> & v2)
	//{
	//	return v1.xp*v2.xp, v1.yp*v2.yp, v1.zp*v2.zp;
	//}

	//template<typename T>
	//static Point3D<T> crossProduct(const Point3D<T> & v1, const Point3D<T> & v2)
	//{
	//	return Point3D<T>(
	//		v1.yp * v2.zp - v1.zp * v2.yp,
	//		v1.zp * v2.xp - v1.xp * v2.zp,
	//		v1.xp * v2.yp - v1.yp * v2.xp);
	//}
	friend class AABB;
};

template<typename T> std::ostream & operator<<(std::ostream & os,const Point3D<T> & p)
{
	os << "[" << p.x() << "," << p.y() << "," << p.z() << "]";
	return os;
}

template<typename T,typename  U> inline Point3D<T> operator*(const U & s, const Point3D<T> & p)
{
    return p * s;
}


typedef Point2D<int> Point2Di;
typedef Point2D<Float> Poin2Df;
typedef Vector2D<int> Vector2Di;
typedef Vector2D<Float> Vector2Df;
typedef Point3D<int> Point3Di;
typedef Point3D<Float> Point3Df;
typedef Vector3D<int> Vector3Di;
typedef Vector3D<Float> Vector3Df;

typedef QMatrix4x4 Trans3DMat;

typedef Vector3D<Float> Vector3f;
typedef Point3D<Float> Point3f;
typedef Vector2D<Float> Vector2f;
typedef Point2D<Float> Point2f;

typedef Point2D<int> Point2i;


typedef Point3f Color;

/*
*
*/

class Ray
{
    Point3f m_o;
    Vector3f m_d;
    Float m_tMax;
    bool m_negDirection[3];
public:
    Ray(const Vector3f & d, const Point3f & o, Float t = MAX_Float_VALUE)noexcept : m_o(o), m_d(d), m_tMax(t)
    {
        m_negDirection[0] = d.x() < 0;
        m_negDirection[1] = d.y() < 0;
        m_negDirection[2] = d.z() < 0;
    }
    Point3f operator()(float t)noexcept { return m_o + t * m_d; }
    const Point3f & original()const {
        return m_o;
    }
    const Vector3f & direction()const {
        return m_d;
    }
    void setMaxLength(Float t) {
        m_tMax = t;
    }
    friend class AABB;
    friend class Triangle;
    friend class Shape;
    friend class BVHTreeAccelerator;
};

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
        m_min(std::min(p0.x, p1.x), std::min(p0.y(), p1.y), std::min(p0.z(), p1.z)),
        m_max(std::max(p0.x, p1.x), std::max(p0.y(), p1.y), std::max(p0.z(), p1.z))
    {}
    AABB(const Point3f & p) :m_min(p), m_max(p) {}

    const Point3f & operator[](int i)const {
        assert(i >= 0 && i < 2);
        if (i == 0)return m_min;
        if (i == 1)return m_max;
        return Point3f();
    }

    Point3f & operator[](int i)noexcept {
        return const_cast<Point3f&>(static_cast<const AABB &>(*this)[i]);
    }

    Point3f corner(int i)const {
        return Point3f(
            (*this)[i & 1].x(), (*this)[i & 2].y(), (*this)[i & 4].z()
        );
    }
    /*
    * Check whether it is intersected with a ray.
    * hit0 stores the nearest ray
    * parameter and hit1 stores the farthest ray parameter
    */
    /*inline function definitions for AABB*/
    bool intersect(const Ray & ray, Float * hit0 = nullptr, Float * hit1 = nullptr) const noexcept;

    Point3f center()const {
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
        return (p.x() >= m_min.x() && p.y() <= m_max.x() &&
            p.y() >= m_min.y() && p.y() <= m_max.y() &&
            p.z() >= m_min.z() && p.z() <= m_max.z());
    }

    bool insideEx(const Point3f & p) const
    {
        return (p.x() >= m_min.x() && p.y() < m_max.x() &&
            p.y() >= m_min.y() && p.y() < m_max.y() &&
            p.z() >= m_min.z() && p.z() < m_max.z());
    }

    /*
    * Check whether the bounding box is
    * intersected with another bounding box
    */

    bool isIntersectWith(const AABB & b);
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
    Vector3f d = diagnal();
    if (d[0] < 0 || d[1] < 0 || d[2] < 0)return Float(0);
    Float area = (d[0] * d[1] + d[1] * d[2] + d[2] * d[0]) * 2;
    return area;
}

inline 
bool AABB::isIntersectWith(const AABB & b)
{
    return (m_max.x() >= b.m_min.x() && b.m_max.x() >= m_min.x()) &&
        (m_max.y() >= b.m_min.y() && b.m_max.y() >= m_min.y()) &&
        (m_max.z() >= b.m_min.z() && b.m_max.z() >= m_min.z());
}

inline 
AABB AABB::unionWith(const Point3f & p)const {
    AABB a;
    a.m_min = Point3f(
        std::min(m_min.x(), p.x()),
        std::min(m_min.y(), p.y()),
        std::min(m_min.z(), p.z())
    );
    a.m_max = Point3f(
        std::max(m_max.x(), p.x()),
        std::max(m_max.y(), p.y()),
        std::max(m_max.z(), p.z())
    );
    return a;
}

inline 
AABB AABB::unionWith(const AABB & b) const
{
    AABB a;
    a.m_min = Point3f(
        std::min(m_min.x(), b.m_min.x()),
        std::min(m_min.y(), b.m_min.y()),
        std::min(m_min.z(), b.m_min.z())
    );
    a.m_max = Point3f(
        std::max(m_max.x(), b.m_max.x()),
        std::max(m_max.y(), b.m_max.y()),
        std::max(m_max.z(), b.m_max.z())
    );
    return a;
}
inline
AABB AABB::intersectWith(const AABB & b) const
{
    return AABB(Point3f(
        std::max(m_min.x(), b.m_min.x()),
        std::max(m_min.y(), b.m_min.y()),
        std::max(m_min.z(), b.m_min.z())
    ),
        Point3f(
            std::min(m_max.x(), b.m_max.x()),
            std::min(m_max.y(), b.m_max.y()),
            std::min(m_max.z(), b.m_max.z())
        ));
}




#endif // GEOMETRY_H

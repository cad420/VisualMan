
#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include "basetype.h"
#include "geometry.h"


#define _USE_TRANSFORMATION		// This Macro will determine conversion methods should be compiled.

#ifdef _USE_TRANSFORMATION
#include "transformation.h"
#endif


namespace ysl
{
	class Quaternion
	{
	public:
		Vector3f v;
		Float w;

	public:
		Quaternion():v(0,0,0),w(0){}
		Quaternion(Float w, const Vector3f & v) :v(v), w(w) {}

#ifdef _USE_TRANSFORMATION
		/*
		 * When we need conversion between Quaternion and Transform, we don't need to implement
		 * corresponding conversion methods in both classes so as to avoid the header file circular 
		 * dependency. What just needed is give a constructor receiving a Transform instance and 
		 * a conversion function only in one side.
		 */

		Quaternion(const Transform & t)
		{
			
		}


		Transform ToTransform()const
		{
			Matrix3x3 m = {
				1.0-2*(v.y*v.y + v.z*v.z),2*(v.x*v.y+v.z*w),2*v.x*v.z-v.y*w,
				2*(v.x*v.y-v.z*w),1-2*(v.x*v.x-v.z*v.z),2*(v.y*v.z + v.x*w),
				2*(v.x*v.z+v.y*w),2*(v.y*v.z-v.x*w),1-2*(v.x*v.x+v.y*v.y)
			};
			return Transform(Matrix4x4(m));
		}
#endif

		Quaternion & operator+=(const Quaternion & q)
		{
			v += q.v;
			w += q.w;
			return *this;
		}

		Quaternion & operator-=(const Quaternion & q)
		{
			v -= q.v;
			w -= q.w;
			return *this;
		}

		Quaternion Mul(const Quaternion & q)
		{

		}
	};


	inline 
	Quaternion operator+(const Quaternion & q1,const Quaternion & q2)
	{
		return { q1.w + q2.w,q1.v + q2.v };
	}

	inline
	Quaternion operator/(const Quaternion & q,const Float & s)
	{
		return { q.w / s, q.v / s };
	}

	inline 
	Quaternion operator-(const Quaternion & q1,const Quaternion & q2)
	{
		return { q1.w - q2.w,q1.v - q2.v };
	}


	inline 
	Float Dot(const Quaternion & q1,const Quaternion & q2)
	{
		return q1.w * q2.w + Vector3f::Dot(q1.v,q2.v);
	}

	inline 
	Quaternion Normalize(const Quaternion & q)
	{
		return q / std::sqrt(Dot(q, q));
	}


}

#endif
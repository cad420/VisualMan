
#ifndef _RAY_H
#define _RAY_H
#include "../mathematics/basetype.h"
#include "../mathematics/geometry.h"
#include <limits>

namespace ysl
{

	class Ray
	{
		Point3f m_o;
		Vector3f m_d;
		Float m_tMax;
		bool m_negDirection[3];
	public:
		Ray(const Vector3f& d, const Point3f& o, Float t = (std::numeric_limits<float>::max)());
		Point3f operator()(float t)const noexcept { return m_o + t * m_d; }
		const Point3f & original()const { return m_o; }
		const Vector3f & direction()const { return m_d; }
		void setMaxLength(Float t) { m_tMax = t; }

		friend class Bound3f;
		friend class Triangle;
		friend class Shape;
		friend class BVHTreeAccelerator;
	};
}

#endif /*_RAY_H*/
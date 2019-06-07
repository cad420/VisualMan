#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "shape.h"

#include "../common/basetype.h"

namespace ysl
{
	class Sphere :public Shape {
		Point3f m_center;
		Float m_radius;
		Bound3f m_bound;
		Float m_area;
	public:
		Sphere(const Point3f & c, Float r, std::shared_ptr<Material> m = nullptr)
			:Shape(m),
			m_center(c),
			m_radius(r),
			m_area(4 * Pi*r*r),
			m_bound(Point3f(c[0] - r, c[1] - r, c[2] - r), Point3f(c[0] + r, c[1] + r, c[2] + r)) {
		}
		bool intersect(const Ray & ray, Float * t, Interaction * iterac)const override;
		Bound3f bound()const override { return m_bound; }
		Float area()const override { return m_area; }
		//Interaction sample(const Interaction & ref, const Point2f & u)const override;
		Interaction sample(const Point2f & u)const override;
	};
}

#endif /*_SPHERE_H_*/
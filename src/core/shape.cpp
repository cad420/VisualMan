#include "shape.h"
#include "interaction.h"

#include "../core/geometry.h"

namespace ysl {




	//bool Sphere::intersect(const Ray & ray, Float * t, Interaction * iterac) const
	//{

	//    return false;
	//}

	//bool Sphere::intersect(const Ray & ray, Float * t, Interaction * iterac) const
	//{
	//	//const Vector3f & D = ray.direction();
	//	//const Vector3f & CO = ray.original() - m_center;
	//	//Float a = Vector3f::dotProduct(D, D);
	//	//Float b = 2 * Vector3f::dotProduct(D, CO);
	//	//Float c = Vector3f::dotProduct(CO, CO) - m_radius * m_radius;
	//	//Float t1, t2;
	//	//Float eps = 0.00001;
	//	//if (quadraticEquation(a, b, c, t1, t2) == false)return false;
	//	//if (t1 > t2)std::swap(t1, t2);
	//	//if (t1 < eps) {
	//	//    if (t2 < eps)return false;
	//	//    if (t)*t = t2;
	//	//    return true;
	//	//}
	//	//if (t)*t = t1;
	//	//return true;
	//	Vector3f rd = ray.direction().normalized();
	//	Vector3f op = m_center - ray.original();
	//	Float res, eps = 1e-4;
	//	Float b = Vector3f::dotProduct(op, rd);
	//	Float det = b * b - Vector3f::dotProduct(op, op) + m_radius * m_radius;
	//	if (det < 0)
	//		return false;
	//	else
	//		det = std::sqrt(det);
	//	if ((res = (b - det)) > eps) {
	//		if (t)*t = res;
	//		return true;
	//	}
	//	else if ((res = (b + det)) > eps) {
	//		if (t)*t = res;
	//		return true;
	//	}
	//	return false;
	//}

	//Interaction Sphere::sample(const Point2f & u) const
	//{
	//	Interaction inter;
	//	Vector3f norm = uniformSampleSphere(u);
	//	//point
	//	inter.m_p = norm + m_center;
	//	//norm
	//	inter.m_norm = norm;
	//	inter.m_pShape = this;

	//	return inter;
	//}

}
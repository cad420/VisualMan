#ifndef INTERACTION_H
#define INTERACTION_H
#include <memory>

#include "../mathematics/geometry.h"



namespace ysl {

	class Shape;
	class BSDF;

	class Interaction
	{
		Point3f  m_p;
		Vector3f  m_wo;
		Vector3f m_norm;
		Vector3f m_t;
		Vector3f m_s;
		Float m_u;
		Float m_v;
		const Shape * m_pShape;
		std::shared_ptr<BSDF> m_bsdf;
		//BRDF
	public:
		Interaction() :m_pShape(nullptr), m_bsdf(nullptr) {}
		Interaction(const Point3f & p,
			const Vector3f & wo,
			Float u, Float v,
			const Vector3f &norm,
			const Vector3f & t,
			const Vector3f & s,
			const Shape * shape,
			const std::shared_ptr<BSDF> bsdf
		):m_p(p), m_wo(wo), m_u(u), m_v(v), m_norm(norm), m_t(t), m_s(s), m_pShape(shape), m_bsdf(bsdf) {}

		const Point3f & intersectionPoint()const { return m_p; }

		const Vector3f & reflectDirection()const { return m_wo; }

		const Vector3f & normal()const { return m_norm; }

		const Shape * object()const { return m_pShape; }

		Ray spawnRay(const Vector3f& dir) const;

		Ray spawnRayTo(const Interaction& ref) const;

		Float u()const { return m_u; }
		Float v()const { return m_v; }
		const Vector3f & t()const { return m_t; }
		const Vector3f & s()const { return m_s; }
		const std::shared_ptr<BSDF> & bsdf()const { return m_bsdf; }

		friend class Shape;
		friend class Triangle;
		friend class Material;
		friend class Sphere;
		friend class AreaLight;
	};
}

#endif // INTERACTION_H

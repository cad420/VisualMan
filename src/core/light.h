
#ifndef _LIGHT_H_
#define _LIGHT_H_


#include "interaction.h"
#include "shape.h"
#include "material.h"

namespace ysl
{

	class Scene;

	class VisibilityTester {
		Interaction m_p0, m_p1;
	public:
		VisibilityTester() {}
		VisibilityTester(const Interaction & p0, const Interaction & p1) :m_p0(p0), m_p1(p1) {}
		const Interaction & from()const { return m_p0; }
		const Interaction & to()const { return m_p1; }
		Float distanceSquare()const;
		bool occlude(const Scene & scene)const;
	};


	class Light {
	public:
		virtual RGBASpectrum sampleLi(const Interaction & ref, Vector3f * wi, Float*pdf, const Point2f &u, VisibilityTester * vis)const = 0;
		virtual Float pdfLi(const Interaction & ref, const Vector3f & wi)const = 0;
		virtual ~Light() {}
	};
	class AreaLight :Light {
		std::shared_ptr<Shape> m_shape;
		RGBASpectrum m_intensity;

	public:
		explicit AreaLight(const std::shared_ptr<Shape> & s, const RGBASpectrum & c) :m_shape(s), m_intensity(c) {}

		explicit AreaLight(const std::shared_ptr<Shape> & s) :m_shape(s) {
			if (m_shape->getMaterial() != nullptr)
				m_intensity = m_shape->getMaterial()->m_ke;
			else
				m_intensity = RGBASpectrum(0.0);
		}
		//sample a light to a surface point from other object, giving direction and pdf
		virtual RGBASpectrum sampleLi(const Interaction & ref, Vector3f * wi, Float*pdf, const Point2f &u, VisibilityTester * vis)const override {

			//shape will overlook the ref
			//see the definition of Shape
			Interaction isectWithLight = m_shape->sample(ref, u);
			*wi = (isectWithLight.m_p - ref.m_p).Normalized();
			*pdf = m_shape->pdf(ref, *wi);

			///TODO:Visibility Test
			*vis = VisibilityTester(ref, isectWithLight);
			return L(isectWithLight, -*wi);
		}
		//evaluate the pdf of the sampling a light along wi from other surface point
		virtual Float pdfLi(const Interaction & ref, const Vector3f & wi)const override { return m_shape->pdf(ref, wi); }

		RGBASpectrum L(const Interaction & isect, const Vector3f & w)const 
		{
			return m_intensity;
			//return Vector3f::dotProduct(isect.m_norm, w) < 0 ? m_intensity : RGBASpectrum(0, 0, 0);
		}
		virtual ~AreaLight() {}
	};
}
#endif

#ifndef _MATERIAL_H_
#define _MATERIAL_H_
#include "spectrum.h"


namespace ysl
{

	enum class MaterialType {
		Glass,
		Mirror,
		Metal
	};


	class Interaction;
	class Shape;
	class Material
	{
	public:
		RGBASpectrum m_kd;
		RGBASpectrum m_ks;
		RGBASpectrum m_ka;
		RGBASpectrum m_ke;
		RGBASpectrum m_tf;
		Float m_ni;
		Float m_ns;
		RGBASpectrum m_color;
		const Shape * m_pShape;
		MaterialType m_type;
	public:
		Material(const RGBASpectrum &kd, const RGBASpectrum &ks, const RGBASpectrum &ka, const RGBASpectrum & ke, const RGBASpectrum &tf, Float ni, Float ns, MaterialType type) :
			m_kd(kd), m_ks(ks), m_ka(ka), m_ke(ke), m_tf(tf), m_ni(ni), m_ns(ns), m_type(type) {}
		void computeScatteringFunction(Interaction * isect);
	};




}

#endif


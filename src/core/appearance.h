
#ifndef _APPEARANCE_H_
#define _APPEARANCE_H_
#include "../mathematics/basetype.h"
#include "../mathematics/geometry.h"

namespace ysl
{
	enum BSDFType {
		BSDF_DIFFUSE = 1 << 0,
		BSDF_SPECULAR = 1 << 1,
		BSDF_REFRACTION = 1 << 2,
		BSDF_ALL = BSDF_DIFFUSE | BSDF_SPECULAR | BSDF_REFRACTION
	};

	class BSDF
	{
		Vector3f m_n, m_t, m_s;
		BSDFType m_type;
		RGBASpectrum m_ka;
		RGBASpectrum m_ks;
		RGBASpectrum m_kd;
		Float m_ni;
		Float m_ns;
		RGBASpectrum m_tf;
		RGBASpectrum m_e;
		//n,m,s are normal and orthognal vectors
		//normal vector in shading coordinate system is (0,1,0)
	public:
		BSDF(const RGBASpectrum & kd,
			const RGBASpectrum & ks,
			const RGBASpectrum & ka,
			const RGBASpectrum & tf,
			Float ni,
			Float ns,
			const RGBASpectrum & e,
			const Vector3f & n,
			const Vector3f & t,
			const Vector3f & s,
			BSDFType type) :m_kd(kd), m_ka(ka), m_ks(ks), m_ni(ni), m_tf(tf), m_ns(ns), m_e(e), m_n(n.normalized()), m_t(t.normalized()), m_s(s.normalized()), m_type(type) {
			createCoordinateSystem(m_n, m_t, m_s);
		}
		//RGBASpectrum f(const Vector3f & wo, const Vector3f & wi)const { return m_color; }
		Vector3f worldToLocal(const Vector3f & v)const;
		Vector3f localToWorld(const Vector3f & v)const;
		void createCoordinateSystem(const Vector3f &N, Vector3f &t, Vector3f &s);
		RGBASpectrum sampleF(const Vector3f & wo, Vector3f * wi, Float *pdf, const Point2f & sample, BSDFType type);
		RGBASpectrum emmision()const { return m_e; }
		BSDFType type()const { return m_type; }
		bool isType(BSDFType type) { return m_type == type; }
	private:
		void fersnel(Float cosTheta, Float n, Float * fr, Float * ft);
	};
}
#endif

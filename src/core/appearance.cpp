#include "appearance.h"
#include "sphere.h"

#include "../mathematics/arithmetic.h"

namespace ysl {
	Vector3f BSDF::worldToLocal(const Vector3f & v) const
	{
		return Vector3f(Vector3f::Dot(v, m_s), Vector3f::Dot(v, m_n), Vector3f::Dot(v, m_t));
	}

	Vector3f BSDF::localToWorld(const Vector3f & v) const
	{
		return Vector3f{
			m_s.x*v.x + m_n.x*v.y + m_t.x*v.z,
			m_s.y*v.x + m_n.y*v.y + m_t.y*v.z,
			m_s.z*v.x + m_n.z*v.y + m_t.z*v.z
		};
	}

	void BSDF::createCoordinateSystem(const Vector3f & N, Vector3f & t, Vector3f & s)
	{
		if (std::fabs(N[0]) > std::fabs(N[1]))
			t = Vector3f(N[2], 0, -N[0]) / sqrtf(N[0] * N[0] + N[2] * N[2]);
		else
			t = Vector3f(0, -N[2], N[1]) / sqrtf(N[1] * N[1] + N[2] * N[2]);
		s = Vector3f::Cross(N, t);
	}

	RGBASpectrum BSDF::sampleF(const Vector3f & wo, Vector3f * wi, Float *pdf, const Point2f & sample, BSDFType type)
	{
		if (isType(type) == false)return RGBASpectrum(0.f);
		RGBASpectrum f;
		//norm must be normalized
		const auto & norm = m_n;
		switch (type)
		{
		case BSDF_DIFFUSE:
		{
			*wi = localToWorld(uniformSampleHemiSphere(sample));
			if (pdf)*pdf = 1.0 / (2 * Pi);
			//Vector3f ref = reflection(norm, -wo);
			//createCoordinateSystem(ref, m_t, m_s);
			//Vector3f sv = cosineSampleHemiSphereWithShiness(sample, m_ni);
			//sv = localToWorld(sv);
			//*wi = sv;
			if (pdf)*pdf = 1;
			return m_kd;
		}
		break;
		case BSDF_SPECULAR:
		{
			//Float s = Vector3f::dotProduct(norm, wo);
			//if(s < 0)return RGBASpectrum(0,0,0);
			//*wi = 2 * (s)*norm-wo;
			Vector3f ref = reflection(norm, -wo);
			createCoordinateSystem(ref, m_t, m_s);
			Vector3f sv = cosineSampleHemiSphereWithShiness(sample, m_ni);
			sv = localToWorld(sv);
			*wi = sv;
			if (pdf)*pdf = 1;
			return m_ks;
		}
		break;
		case BSDF_REFRACTION:
		{
			Float cosTheta = Vector3f::Dot(m_n, -wo);
			Float n;
			Float fr, ft;
			Vector3f realNorm = m_n;
			if (pdf)*pdf = 1;
			if (cosTheta < 0) {
				//outer
				n = 1.0 / m_ni;
				cosTheta = -cosTheta;
				//tracing reflection and refraction
			}
			else {
				//inter
				realNorm = -realNorm;
				n = m_ni;
				//assert(n <= 1.0);
			}
			fersnel(cosTheta, n, &fr, &ft);
			Vector3f refrac = refraction(realNorm, -wo, n);
			Float s;
			if (refrac.IsNull() == false) {
				if (russianRoulette(fr, sample[0]) == true) {
					*wi = reflection(m_n, -wo);
					return m_ks;
				}
				else {
					*wi = refrac;
					return m_ks;
				}
			}
			else {
				*wi = reflection(m_n, -wo);
				return m_ks;
			}

		}
		break;
		case BSDF_ALL:
			assert(false);
			break;
		default:
			assert(false);
			break;
		}
		return f;
	}

	void BSDF::fersnel(Float cosTheta, Float n, Float *fr, Float *ft)
	{
		Float f0 = (n - 1)*(n - 1) / ((n + 1)*(n + 1));
		if (fr)*fr = f0;
		if (ft)*ft = f0 + (1 - f0)*std::pow(1 - cosTheta, 5.0);
	}


}


#include "material.h"
#include "../accelerator/bvh.h"
#include "appearance.h"


namespace ysl
{

	void Material::computeScatteringFunction(Interaction * isect)
	{
		BSDFType bsdfType;
		switch (m_type)
		{
		case MaterialType::Glass:
			bsdfType = BSDF_REFRACTION;
			break;
		case MaterialType::Mirror:
			bsdfType = BSDF_SPECULAR;
			break;
		case MaterialType::Metal:
			bsdfType = BSDF_DIFFUSE;
			break;
		default:
			//error
			assert(false);
			break;
		}
		isect->m_bsdf = std::make_shared<BSDF>(m_kd, m_ks, m_ka, m_tf, m_ni, m_ns, m_ke, isect->m_norm, isect->m_t, isect->m_s, bsdfType);
	}

}

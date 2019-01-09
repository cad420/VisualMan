
#include "ray.h"

namespace ysl
{
	Ray::Ray(const Vector3f& d, const Point3f& o, Float t): m_o(o), m_d(d), m_tMax(t)
	{
		m_negDirection[0] = d.x < 0;
		m_negDirection[1] = d.y < 0;
		m_negDirection[2] = d.z < 0;
	}
}
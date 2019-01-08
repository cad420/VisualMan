#include "interaction.h"

namespace  ysl
{
	Ray Interaction::spawnRay(const Vector3f& dir) const
	{
		return Ray(dir.Normalized(), m_p + 0.0001 * (m_norm * Vector3f::Dot(dir, m_norm)).Normalized());
	}

	Ray Interaction::spawnRayTo(const Interaction& ref) const
	{
		Vector3f dir = (ref.m_p - m_p).Normalized();
		return Ray(dir, m_p + 0.0001 * (m_norm * Vector3f::Dot(dir, m_norm)).Normalized());
	}
}

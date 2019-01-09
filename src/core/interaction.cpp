#include "interaction.h"

namespace  ysl
{
	Ray Interaction::spawnRay(const Vector3f& dir) const
	{
		return { dir.Normalized(), 
			m_p + 0.0001 * (m_norm * Vector3f::Dot(dir, m_norm)).Normalized() };
	}

	Ray Interaction::spawnRayTo(const Interaction& ref) const
	{
		const auto dir = (ref.m_p - m_p).Normalized();
		return { dir, m_p + 0.0001 * (m_norm * Vector3f::Dot(dir, m_norm)).Normalized() };
	}
}

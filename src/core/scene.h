#ifndef SCENE_H
#define SCENE_H

#include "shape.h"
#include <vector>

namespace ysl {
#include <memory>
	class AreaLight;
	class Scene
	{
		AABB worldBound;
		std::shared_ptr<Shape> primitive;
		std::vector<std::shared_ptr<AreaLight>> m_lights;
	public:
		Scene(std::shared_ptr<Shape> shape, const std::vector<std::shared_ptr<AreaLight>> & lights) :primitive(shape), m_lights(lights) {}

		bool intersect(const Ray & ray, Float * t, Interaction * isect)const
		{
			return primitive->intersect(ray, t, isect);
		}
		const std::vector<std::shared_ptr<AreaLight>> & lights()const { return m_lights; }
		void SetObject(std::shared_ptr<Shape> shape) {primitive = shape;}
	};
}


#endif // SCENE_H

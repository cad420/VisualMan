#ifndef SCENE_H
#define SCENE_H

#include "shape.h"
#include <vector>

namespace ysl {


#include <memory>
	class AreaLight;

	class Scene
	{
		AABB m_worldBound;
		std::shared_ptr<Shape> m_shape;
		std::vector<std::shared_ptr<AreaLight>> m_lights;
	public:
		Scene(std::shared_ptr<Shape> shape, const std::vector<std::shared_ptr<AreaLight>> & lights) :m_shape(shape), m_lights(lights) {}

		bool intersect(const Ray & ray, Float * t, Interaction * isect)const 
		{
			return m_shape->intersect(ray, t, isect);
		}
		const std::vector<std::shared_ptr<AreaLight>> & lights()const { return m_lights; }
		void setObject(std::shared_ptr<Shape> shape) {
			m_shape = shape;
		}
	};

}
#endif // SCENE_H

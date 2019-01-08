#ifndef BVH_H
#define BVH_H

#include "../mathematics/geometry.h"
#include "../core/shape.h"

#include <mutex>
#include <vector>

namespace ysl {

	class Interaction;

	class BVHTreeAccelerator :public Shape
	{
		class BVHNode {
		public:
			int m_nShape;
			int m_shapeOffset;
			int m_splitAxis;
			AABB m_bound;
			std::unique_ptr<BVHNode> m_left;
			std::unique_ptr<BVHNode> m_right;
			BVHNode() {}
			BVHNode(std::unique_ptr<BVHNode> &&left, std::unique_ptr<BVHNode> &&right, int shapeOffset,
				int splitAxis,
				int nshape,
				const AABB & b = AABB()) :
				m_left(std::move(left)),
				m_right(std::move(right)),
				m_nShape(nshape),
				m_shapeOffset(shapeOffset),
				m_splitAxis(splitAxis),
				m_bound(b) {}
		};
		std::vector<std::shared_ptr<Shape>> m_shapes;
		std::unique_ptr<BVHNode> m_root;
		//mutable std::mutex m_m_tMinMutex;
		//mutable Float m_tMin;

		//mutable bool m_debug_isect;
		//mutable bool m_debug_flag;

	public:
		BVHTreeAccelerator(std::vector<std::shared_ptr<Shape>> & shapes) :m_shapes(shapes), Shape(nullptr) {
			std::vector<std::shared_ptr<Shape>> orderedShapes;
			int s = m_shapes.size();
			m_root = recursiveBuild(m_shapes, 0, s, orderedShapes);
			m_shapes.swap(orderedShapes);
		}
		bool intersect(const Ray & ray, Float * t, Interaction * interac)const override {

			//std::unique_lock<std::mutex> lock(m_m_tMinMutex);
			//m_tMin = MAX_Float_VALUE;
			//m_debug_isect = false;
			//m_debug_flag = false;
			Float tMin = MAX_Float_VALUE;
			bool intersect = recursiveIntersect(m_root.get(), ray, interac, tMin);
			//assert(!(m_debug_isect == true && m_debug_flag == false));
			if (intersect == true) {
				if (t)*t = tMin;
				return true;
			}
			return false;
		}
		AABB bound()const override {
			return AABB();
		}
		Float area()const override {
			return Float(0);
		}
	private:

		bool recursiveIntersect(const BVHNode * root, const Ray & ray, Interaction * interac, Float &tMin)const;

		std::unique_ptr<BVHNode> recursiveBuild(std::vector<std::shared_ptr<Shape>> & shapes,
			int begin, int end,
			std::vector<std::shared_ptr<Shape>> & orderedShapes);
	};

}

#endif // BVH_H


#include "bvh.h"
#include "../core/interaction.h"
#include "../core/arithmetic.h"


namespace ysl {


	bool BVHTreeAccelerator::recursiveIntersect(const BVHNode * root, const Ray & ray, Interaction * interac, Float &tMin)const
	{

		//If the BVH is empty or there is no intersection with current node
		if (root == nullptr)
			return false;
		if (root->m_bound.intersect(ray, nullptr, nullptr) == false)
			return false;
		//Interior node
		if (root->m_nShape == -1) {
			///TODO:This is need to be optimazed 
			int splitAxis = root->m_splitAxis;
			const AABB & bLeft = root->m_left->m_bound;
			const AABB & bRight = root->m_right->m_bound;
			bool interLeft, interRight;
			if (ray.m_negDirection[splitAxis] == false)
			{
				if (bLeft.m_min[splitAxis] <= bRight.m_min[splitAxis])
				{
					interLeft = recursiveIntersect(root->m_left.get(), ray, interac, tMin);
					interRight = recursiveIntersect(root->m_right.get(), ray, interac, tMin);
				}
				else
				{
					interRight = recursiveIntersect(root->m_right.get(), ray, interac, tMin);
					interLeft = recursiveIntersect(root->m_left.get(), ray, interac, tMin);
				}
			}
			else
			{
				if (bLeft.m_min[splitAxis] > bRight.m_min[splitAxis])
				{
					interLeft = recursiveIntersect(root->m_left.get(), ray, interac, tMin);
					interRight = recursiveIntersect(root->m_right.get(), ray, interac, tMin);
				}
				else
				{
					interRight = recursiveIntersect(root->m_right.get(), ray, interac, tMin);
					interLeft = recursiveIntersect(root->m_left.get(), ray, interac, tMin);
				}
			}
			return interLeft || interRight;
		}
		else {
			//Leaf node, check and find the nearest intersection
			bool isect = false;
			Float tWithBound;
			Float tWithShape;

			//test bool
			bool flag = false;

			for (int i = 0; i < root->m_nShape; i++) {
				if (m_shapes[i + root->m_shapeOffset]->bound().intersect(ray, &tWithBound) == true) {
					if (tMin < tWithBound)continue;
					Interaction inter;
					if (m_shapes[i + root->m_shapeOffset]->intersect(ray, &tWithShape, &inter) == true)
					{
						//assert(!std::isnan(tWithShape));
						//qDebug() << "tmin" << m_tMin << " " << tWithShape;
						if (tMin > tWithShape)
						{
							tMin = tWithShape;
							*interac = inter;
							//m_debug_flag = true;
						}
						isect = true;
						//m_debug_isect = true;
					}
				}
			}
			//qDebug() << "m_tMin:" << m_tMin << " tWithShape:" << tWithShape;

			//assert(!(isect == true && flag == false));
			return isect;
		}
	}

	std::unique_ptr<BVHTreeAccelerator::BVHNode>
		BVHTreeAccelerator::recursiveBuild(std::vector<std::shared_ptr<Shape>> & shapes,
			int begin,
			int end,
			std::vector<std::shared_ptr<Shape>> & orderedShapes)
	{
		const auto currentNodeCount = end - begin;
		const auto offset = orderedShapes.size();
		AABB bound;
		for (int i = begin; i < end; i++) {
			AABB b = shapes[i]->bound();
			bound = bound.unionWith(shapes[i]->bound());
		}
		Vector3f boundDiag = bound.diagnal();
		const auto splitAxis = findMaxVector3fComponent(boundDiag);
		if (currentNodeCount == 1) {
			//create leaf node and return
			std::unique_ptr<BVHNode> newNode(new BVHNode(nullptr, nullptr, offset, splitAxis, currentNodeCount, bound));
			for (int i = begin; i < end; i++)
			{
				orderedShapes.push_back(m_shapes[i]);
			}
			return newNode;
		}
		else {
			AABB centroidBound;
			for (int i = begin; i < end; i++) {
				centroidBound = centroidBound.unionWith(shapes[i]->bound().center());
			}
			/*
			* if the maximum component of centroids of the
			* bounds of all shapes are the same,
			* just create a leaf node for all of them
			*/

			if (centroidBound.m_min[splitAxis] == centroidBound.m_max[splitAxis]) {
				std::unique_ptr<BVHNode> newNode(new BVHNode(nullptr, nullptr, offset, splitAxis, currentNodeCount, bound));
				for (int i = begin; i < end; i++)
				{
					orderedShapes.push_back(m_shapes[i]);
				}

				return newNode;
			}

			//find the split position with minimum cost
			struct bucketInfo {
				std::vector<int> indices;
				AABB bound;
			};
			constexpr int nBuckets = 12;
			bucketInfo buckets[nBuckets];

			for (int i = begin; i < end; i++) {
				Point3f center = shapes[i]->bound().center();
				int bucketIndex = nBuckets * ((center[splitAxis] - bound.m_min[splitAxis]) / boundDiag[splitAxis]);
				if (bucketIndex == nBuckets)bucketIndex -= 1;
				buckets[bucketIndex].indices.push_back(i);
				buckets[bucketIndex].bound = buckets[bucketIndex].bound.unionWith(shapes[i]->bound());
			}
			Float cost[nBuckets - 1];
			for (int i = 0; i < nBuckets - 1; i++) {
				AABB bLeft, bRight;
				int nLeft = 0, nRight = 0;
				for (int j = 0; j <= i; j++) {
					bLeft = bLeft.unionWith(buckets[j].bound);
					nLeft += buckets[j].indices.size();
				}
				for (int j = i + 1; j < nBuckets; j++) {
					bRight = bRight.unionWith(buckets[j].bound);
					nRight += buckets[j].indices.size();
				}
				cost[i] = 0.125 + (nLeft*bLeft.surfaceArea() +
					bRight.surfaceArea()*nRight) / bound.surfaceArea();
			}
			auto efficientIter = std::min_element(cost, cost + nBuckets - 1);
			int efficentIndex = std::distance(cost, efficientIter);
			if (currentNodeCount > 255 || *efficientIter < currentNodeCount) {
				//if the count of left nodes is greater than the max count of a leaf node can hold
				//or the cost of spliting the current node is less than creating a leaf node's with current nodes
				auto lambda = [&](const std::shared_ptr<Shape> s) {
					int b = nBuckets * ((s->bound().center()[splitAxis] - bound.m_min[splitAxis]) / boundDiag[splitAxis]);
					if (b == nBuckets)b -= 1;
					//equal is necessary. or the partition will be one-side and it never ends
					return b <= efficentIndex;
				};
				int mid = std::distance(shapes.begin(), std::partition(shapes.begin() + begin, shapes.begin() + end, lambda));
				//create interior node
				auto leftNode = recursiveBuild(shapes, begin, mid, orderedShapes);
				auto rightNode = recursiveBuild(shapes, mid, end, orderedShapes);
				std::unique_ptr<BVHNode> newNode(
					new BVHNode(std::move(leftNode),
						std::move(rightNode),
						offset, splitAxis, -1, bound)
				);
				return newNode;
			}
			else {
				//create leaf node and return
				std::unique_ptr<BVHNode> newNode(new BVHNode(nullptr, nullptr, offset, splitAxis, currentNodeCount, bound));
				for (int i = begin; i < end; i++)
				{
					orderedShapes.push_back(m_shapes[i]);
				}

				return newNode;
			}
		}
	}

}

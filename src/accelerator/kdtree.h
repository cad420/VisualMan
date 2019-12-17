
#ifndef _KDTREE_H_
#define _KDTREE_H_
#include "../mathematics/basetype.h"
#include <vector>
#include "../core/shape.h"

namespace vm
{
/**
	 * \brief It's worthwhile to keep the kd tree node small for cache-friendly, 
	 * which will lead a significant performance promotion.
	 * This node struct is only 8 bytes if the size of \a Float is 4 bytes
	 */
struct KdAccelNode
{
	union
	{
		Float split;				 // Interior
		int onePrimitive;			 // Leaf
		int primitiveIndicesOffset;	 // Leaf
	};

	union
	{
		int flags;		 // Both
		int nPrims;		 // Leaf
		int aboveChild;	 // Interior
	};

	void InitLeaf( int *primNums, int np, std::vector<int> *primitiveIndices )
	{
		flags = 3;	// Indicating a Leaf Node
		nPrims |= ( np << 2 );

		if ( np == 0 )
			onePrimitive = 0;
		else if ( np == 1 )
			onePrimitive = primNums[ 0 ];
		else {
			primitiveIndicesOffset = primitiveIndices->size();
			for ( auto i = 0; i < np; i++ ) {
				primitiveIndices->push_back( primNums[ i ] );
			}
		}
	}

	void InitInterior( int axis, int ac, Float s )
	{
		split = s;
		flags = axis;
		aboveChild |= ( ac << 2 );
	}

	Float SplitPos() const { return split; }
	int nPrimitives() const { return nPrims; }
	int SplitAxis() const { return ( flags & 3 ); }
	bool IsLeaf() const { return ( flags & 3 ) == 3; }
	int AboveChild() const { return aboveChild >> 2; }
};

class KdTreeAccelerator : public Shape
{
public:
	KdTreeAccelerator( const std::vector<std::shared_ptr<Shape>> &p,
					   int isectCost,
					   int traversalCost,
					   Float emptyBonus,
					   int maxPrims,
					   int maxDepth );

private:
	const int isectCost, traversalCost, maxPrims;
	const Float emptyBonus;
	std::vector<std::shared_ptr<Shape>> primitives;
	KdAccelNode *root;
	int nAllocatedNodes, nextFreeNode;
	AABB bounds;
};

}  // namespace vm

#endif /*_KDTREE_H_*/
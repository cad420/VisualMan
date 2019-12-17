
#include "kdtree.h"
#include <vector>

namespace vm
{
KdTreeAccelerator::KdTreeAccelerator( const std::vector<std::shared_ptr<Shape>> &p,
									  int isectCost,
									  int traversalCost,
									  Float emptyBonus,
									  int maxPrims,
									  int maxDepth ) :
  Shape( nullptr ),
  isectCost( isectCost ),
  traversalCost( 0 ),
  maxPrims( 0 ),
  emptyBonus( emptyBonus ),
  primitives( p )
{
	nextFreeNode = nAllocatedNodes = 0;
	if ( maxDepth <= 0 )
		maxDepth = std::round( 8 + 1.3f * int( std::log2( primitives.size() ) ) );
	std::vector<AABB> primBounds;
	primBounds.reserve( primitives.size() );
	for ( const auto &p : primitives ) {
		const auto b = p->bound();
		bounds = bounds.UnionWith( b );
		primBounds.push_back( b );
	}

	std::unique_ptr<int[]> primNums( new int[ primitives.size() ] );
	for ( std::size_t i = 0; i < primitives.size(); i++ )
		primNums[ i ] = i;
}
}  // namespace ysl

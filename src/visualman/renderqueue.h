
#ifndef _RENDERQUEUE_H_
#define _RENDERQUEUE_H_

#include <queue>
#include "actor.h"
#include "shading.h"
#include "graphictype.h"
#include <VMFoundation/dataarena.h>

namespace vm
{
class RenderNode
{
public:
	RenderNode() = default;
	RenderNode( Actor *actor, Renderable *renderable, const Shading *shading, RenderNode *nextPass ) :
	  actor( actor ), renderable( renderable ), shading( shading ), nextPass( nextPass ) {}
	Actor *actor = nullptr;
	Renderable *renderable = nullptr;
	const Shading *shading = nullptr;
	RenderNode *nextPass = nullptr;
};

class RenderQueue
{
public:
	RenderQueue() = default;
	RenderNode *CreateRenderNode( Actor *actor, Renderable *renderable, const Shading *shading )
	{
		const auto node = arena.AllocConstruct<RenderNode>( actor, renderable, shading, nullptr );
		nodes.push_back( node );
		return node;
	}

	RenderNode *CreateMultiPass( RenderNode *prePass, Actor *actor, Renderable *renderable, const Shading *shading )
	{
		auto pass = arena.AllocConstruct<RenderNode>( actor, renderable, shading, nullptr );
		assert( prePass );
		prePass->nextPass = pass;
		return pass;
	}
	RenderNode *operator[]( size_t i ) { return nodes[ i ]; }
	const RenderNode *operator[]( size_t i ) const { return nodes[ i ]; }
	void Clear() { nodes.clear(); }
	bool Empty() const { return nodes.empty(); }
	size_t Size() const { return nodes.size(); }

	void Sort();

private:
	std::vector<RenderNode *> nodes;
	DataArena<64> arena;
};

//class GRAPHICS_EXPORT_IMPORT RenderQueueSorter
//{
//public:
//	RenderQueueSorter() = default;
//	virtual bool operator<(const RenderNode * node1, const RenderNode * node2)const = 0;
//};

//class GRAPHICS_EXPORT_IMPORT PriorityRenderQueueSorter
//{
//public:
//	PriorityRenderQueueSorter() = default;
//	bool operator<(const RenderNode * node1,const RenderNode * node2)const
//	{
//
//	}
//};

}  // namespace vm

#endif
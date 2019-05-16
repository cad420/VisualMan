
#ifndef _RENDERQUEUE_H_
#define _RENDERQUEUE_H_

#include <queue>
#include "actor.h"
#include "shading.h"
#include "../memory/dataarena.h"

namespace ysl
{
	namespace graphics
	{
		class RenderNode
		{
		public:
			RenderNode() = default;
			Actor * actor = nullptr;
			Renderable * renderable = nullptr;
			const Shading * shading = nullptr;
			RenderNode * nextPass = nullptr;
		};

		class RenderQueue
		{
		public:
			RenderQueue() = default;
			RenderNode * CreateRenderNode()
		    {
				const auto node = arena.Alloc<RenderNode>(1);
				nodes.push_back(node);
				return node;
		    }

			RenderNode * CreateMultiPass()
			{
				return arena.Alloc<RenderNode>(1);
			}
			RenderNode * operator[](size_t i) { return nodes[i]; }
			void Clear() { nodes.clear(); }
			bool Empty()const { return nodes.empty(); }
			size_t Size()const { return nodes.size(); }


		private:
			std::vector<RenderNode*> nodes;
			DataArena<64> arena;
		};
	}
}

#endif
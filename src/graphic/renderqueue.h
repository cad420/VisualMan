
#ifndef _RENDERQUEUE_H_
#define _RENDERQUEUE_H_

#include <queue>
#include "actor.h"
#include "shaing.h"

namespace ysl
{
	namespace graphics
	{
		class RenderItem
		{
			RenderItem() = default;
		public:
			Actor * actor;
			Renderable * renderable;
			const Shading * shading;
		};


		class GRAPHICS_EXPORT_IMPORT RenderQueue
		{
			public:	
		};
	}
}

#endif
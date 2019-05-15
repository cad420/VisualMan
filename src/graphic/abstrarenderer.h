#ifndef _ABSTRARENDERER_H_
#define _ABSTRARENDERER_H_

#include <vector>
#include "graphictype.h"
#include <memory>
#include "renderqueue.h"
#include "camera.h"
#include "framebuffer.h"
#include "renderable.h"

namespace ysl
{
	namespace graphics
	{
		class GRAPHICS_EXPORT_IMPORT AbstraRenderer
		{
		public:
			AbstraRenderer() = default;
			virtual ~AbstraRenderer() = default;
			virtual void Render(const std::vector<Ref<Actor>> & rederQueue,const Ref<Camera> & camera) = 0;
			
		};
	}
}

#endif
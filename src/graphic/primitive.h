
#ifndef _MESH_H_
#define _MESH_H_
#include "renderable.h"
#include <vector>
#include "graphictype.h"
#include "abstrdraw.h"

namespace ysl
{
	namespace gpu
	{
		class GRAPHICS_EXPORT_IMPORT Primitive:public Renderable
		{
		public:
			Primitive() = default;
			virtual ~Primitive() = default;
			std::vector<Ref<AbstrDrawCall>> DrawCalls()const { return drawCalls; }
		protected:
			void Render_Implement(const Actor * actor, const RenderState* state, const Camera* camera, RenderContext * context)override;
		private:
			std::vector<Ref<AbstrDrawCall>> drawCalls;
		};
	}
}

#endif
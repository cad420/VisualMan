
#ifndef _RENDERABLE_H_
#define _RENDERBALE_H_
#include "graphictype.h"
namespace ysl
{
	namespace graphics
	{
		class RenderContext;
		class Camera;
		class RenderState;
		class Actor;

		class  Renderable
		{
		public:
			Renderable() = default;
			virtual void Render(const Actor * actor, const RenderState* state, const Camera* camera, RenderContext * context);
			virtual ~Renderable() = default;
		protected:
			virtual void Render_Implement(const Actor * actor, const RenderState* state, const Camera* camera, RenderContext * context)const = 0; // like a callback
		};
	}
}

#endif
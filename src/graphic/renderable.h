
#ifndef _RENDERABLE_H_
#define _RENDERBALE_H_
#include "graphictype.h"
namespace ysl
{
	namespace vm
	{
		class Shading;
		class RenderContext;
		class Camera;
		class RenderState;
		class Actor;

		class  Renderable
		{
		public:
			Renderable() = default;
			virtual void Render(const Actor * actor, const Shading * shading, const Camera* camera, RenderContext * context);
			virtual void UpdateDirtyBufferObject(BufferObjectUpdateMode mode) = 0;

			void SetBufferObjectDirty(bool dirty) { renderableDirty = dirty; }
			bool GetBufferObjectDirty()const { return renderableDirty; }

			void SetBufferObjectEnabled(bool enable) { enableBufferObject = enable; }
			bool GetBufferObjectEnabled()const { return enableBufferObject; }

			virtual ~Renderable() = default;
		protected:
			virtual void Render_Implement(const Actor * actor, const Shading * shading, const Camera* camera, RenderContext * context)const = 0; // like a callback
		private:
			bool renderableDirty = true;
			bool enableBufferObject = true;
		};
	}
}

#endif
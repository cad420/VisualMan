
#ifndef _RENDERER_H_
#define _RENDERER_H_
#include "abstrarenderer.h"
#include "renderqueue.h"

namespace vm
{
class Framebuffer;
class TransformChangeCallback;

class VISUALMAN_EXPORT_IMPORT Renderer : public AbstraRenderer
{
public:
	Renderer();
	VMRef<Framebuffer> GetFramebuffer() override { return framebuffer; }
	void SetFramebuffer( VMRef<Framebuffer> fb ) { framebuffer = std::move( fb ); }
	void Render( const RenderQueue &rederQueue, const VMRef<Camera> &camera ) override;

private:
	VMRef<Framebuffer> framebuffer;
	VMRef<TransformChangeCallback> transformCallback;
};

}  // namespace vm

#endif
#ifndef _ABSTRARENDERER_H_
#define _ABSTRARENDERER_H_

#include <memory>
#include <vector>
#include "graphictype.h"
#include "renderqueue.h"
#include "camera.h"
#include "framebuffer.h"

namespace vm
{
class IRenderEvent;

class VISUALMAN_EXPORT_IMPORT AbstraRenderer
{
public:
	AbstraRenderer() = default;
	virtual ~AbstraRenderer() = default;
	virtual void Render( const RenderQueue &rederQueue, const VMRef<Camera> &camera ) = 0;
	virtual VMRef<Framebuffer> GetFramebuffer() = 0;
	void DispatchOnRenderStartedEvent();
	void DispatchOnRenderFinishedEvent();
	void AddRenderStartedEventCallback( VMRef<IRenderEvent> callback );
	void AddRenderFinishedEventCallback( VMRef<IRenderEvent> callback );

private:
	std::vector<VMRef<IRenderEvent>> startedCallbacks;
	std::vector<VMRef<IRenderEvent>> finishedCallbacks;
};

}  // namespace vm

#endif
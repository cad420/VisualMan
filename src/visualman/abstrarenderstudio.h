
#ifndef _ABSTRARENDERSTUDIO_H_
#define _ABSTRARENDERSTUDIO_H_

#include "graphictype.h"
#include "renderevent.h"

#include <vector>

namespace vm
{
class VISUALMAN_EXPORT_IMPORT AbstraAggregate
{
public:
	virtual ~AbstraAggregate() = default;
	virtual void Render() = 0;
	void DispatchRenderStartedEvent();
	void DispatchRenderFinishedEvent();
	void AddRenderStartedEventCallback( VMRef<IRenderEvent> callback );
	void AddRenderFinishedEventCallback( VMRef<IRenderEvent> callback );

protected:
	std::vector<VMRef<IRenderEvent>> renderStartedcallbacks;
	std::vector<VMRef<IRenderEvent>> renderFinishedCallbacks;
};
}  // namespace vm

#endif
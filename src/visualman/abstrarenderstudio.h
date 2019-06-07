
#ifndef _ABSTRARENDERSTUDIO_H_
#define _ABSTRARENDERSTUDIO_H_

#include "graphictype.h"
#include <vector>
#include "renderevent.h"

namespace ysl
{
	namespace vm
	{
		class VISUALMAN_EXPORT_IMPORT AbstraAggregate
		{
		public:
			virtual ~AbstraAggregate()=default;
			virtual void Render() = 0;
			void DispatchRenderStartedEvent();
			void DispatchRenderFinishedEvent();
			void AddRenderStartedEventCallback(Ref<IRenderEvent> callback);
			void AddRenderFinishedEventCallback(Ref<IRenderEvent> callback);
		protected:
			std::vector<Ref<IRenderEvent>> renderStartedcallbacks;
			std::vector<Ref<IRenderEvent>> renderFinishedCallbacks;
		};
	}
}

#endif
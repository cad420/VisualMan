
#ifndef _ABSTRARENDERSTUDIO_H_
#define _ABSTRARENDERSTUDIO_H_

#include "graphictype.h"

namespace ysl
{
	namespace gpu
	{
		class GRAPHICS_EXPORT_IMPORT AbstraRenderStudio
		{
		public:
			virtual ~AbstraRenderStudio()=default;

			virtual void Render() = 0;
			void DispatchRenderStartedEvent();
			void dispatchRenderFinishedEvent();
		protected:
		};
	}
}

#endif
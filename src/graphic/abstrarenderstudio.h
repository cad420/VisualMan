
#ifndef _ABSTRARENDERSTUDIO_H_
#define _ABSTRARENDERSTUDIO_H_

namespace ysl
{
	namespace gpu
	{
		class AbstraRenderStudio
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
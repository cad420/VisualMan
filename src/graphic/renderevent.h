
#ifndef _RENDEREVENT_H_
#define _RENDEREVENT_H_



namespace ysl
{
	namespace vm
	{
		class AbstraFrame;
		class AbstraRenderer;

		class GRAPHICS_EXPORT_IMPORT IRenderEvent
		{
		public:
			virtual bool OnRenderingStartedEvent(const vm::AbstraFrame * studio) = 0;
			virtual bool OnRenderingFinishedEvent(const vm::AbstraFrame * studio) = 0;
			virtual bool OnRendererStartedEvent(const AbstraRenderer * renderer) = 0;
			virtual bool OnRendererFinishedEvent(const AbstraRenderer * renderer) = 0;
			void SetRemoveAfterCall(bool remove) { removeAfterCall = remove; }
			bool IsRemoveAfterCallEnabled()const { return removeAfterCall; }
			void SetEnabled(bool enable) { eventEnable = enable; }
			bool IsEnabled()const { return eventEnable; }
		protected:
			bool removeAfterCall = false;
			bool eventEnable = true;
		};
	}
}

#endif


#ifndef _EVENTINTERFACE_H_
#define _EVENTINTERFACE_H_
#include "graphictype.h"
namespace ysl
{
	namespace graphics
	{
		class RenderContext;
		enum  EMouseButton
		{
			Mouse_Left = 0x01,
			Mouse_Right = 0x02
		};
		enum  EKeyButton
		{
			Key_Up,
			Key_Down,
			Key_Left,
			Key_Right
		};


		class IEventListener
		{
			friend class RenderContext;
		public:
			IEventListener() = default;

			void SetEnable(bool enable) { eventEnabled = enable; }

			bool Enabled()const { return eventEnabled; }

			virtual void InitEvent() = 0;

			virtual void DestroyEvent() = 0;

			virtual void ResizeEvent(int w, int h) = 0;

			virtual void UpdateEvent() = 0;

			/**
			 * \brief Invoked after it was added into a context
			 */
			virtual void AddedEvent(RenderContext * context) = 0;

			/**
			 * \brief Invoked after it was deleted from a context
			 */
			virtual void DeletedEvent(RenderContext * context) = 0;

			virtual void MousePressEvent(EMouseButton button, int xpos, int ypos) = 0;

			virtual void MouseMoveEvent(EMouseButton button, int xpos, int ypos) = 0;

			virtual void MouseReleaseEvent(EMouseButton button, int xpos, int ypos) = 0;

			virtual void MouseWheelEvent(int ydegree,int xdegree) = 0;

			virtual void KeyPressEvent(EKeyButton key) = 0;

			virtual void KeyReleaseEvent(EKeyButton key) = 0;

			RenderContext * Context() { return context; }

			const RenderContext * Context()const { return context; }

			void SetContext(RenderContext * context) { this->context = context; }

			virtual ~IEventListener() = default;
		private:
			RenderContext * context = nullptr;
			bool eventEnabled = true;
		};
	}
}

#endif
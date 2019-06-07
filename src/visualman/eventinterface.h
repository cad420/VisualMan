
#ifndef _EVENTINTERFACE_H_
#define _EVENTINTERFACE_H_
#include "graphictype.h"
namespace ysl
{
	namespace vm
	{
		class RenderContext;
		enum  MouseButton
		{
			Mouse_Left = 0x01,
			Mouse_Right = 0x02
		};
		enum  KeyButton
		{
			Key_0,
			Key_1,
			Key_2,
			Key_3,
			Key_4,
			Key_5,
			Key_6,
			Key_7,
			Key_8,
			Key_9,

			Key_A,
			Key_B,
			Key_C,
			Key_D,
			Key_E,
			Key_F,
			Key_G,
			Key_H,
			Key_I,
			Key_J,
			Key_K,
			Key_L,
			Key_M,
			Key_N,
			Key_O,
			Key_P,
			Key_Q,
			Key_R,
			Key_S,
			Key_T,
			Key_U,
			Key_V,
			Key_W,
			Key_X,
			Key_Y,
			Key_Z,

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

			virtual void MousePressEvent(MouseButton button, int xpos, int ypos) = 0;

			virtual void MouseMoveEvent(MouseButton button, int xpos, int ypos) = 0;

			virtual void MouseReleaseEvent(MouseButton button, int xpos, int ypos) = 0;

			virtual void MouseWheelEvent(int ydegree,int xdegree) = 0;

			virtual void KeyPressEvent(KeyButton key) = 0;

			virtual void KeyReleaseEvent(KeyButton key) = 0;

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

#ifndef _EVENTINTERFACE_H_
#define _EVENTINTERFACE_H_
#include "graphictype.h"
#include <memory>

namespace ysl
{
	namespace gpu
	{

		class RenderContext;

		enum class EMouseButton
		{
			Left,
			Right
		};

		enum class EKeyButton
		{
			
		};

		class IEventListener
		{
		public:
			IEventListener() = default;

			virtual void InitEvent() = 0;
			virtual void DestroyEvent() = 0;
			virtual void UpdateEvent() = 0;
			virtual void MousePressEvent(EMouseButton button, int xpos, int ypos) = 0;
			virtual void MouseReleaseEvent(EMouseButton button, int xpos, int ypos = 0;
			virtual void KeyPressEvent(EKeyButton key) = 0;
			virtual void KeyReleaseEvent(EKeyButton key) = 0;
			virtual void ResizeEvent(int w, int h) = 0;


			WeakPtr(RenderContext) Context()const
			{
				return context;
			}


			virtual ~IEventListener() = default;
		private:
			WeakPtr(RenderContext) context;
		};
	}
}



#endif

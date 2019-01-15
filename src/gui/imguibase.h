
#ifndef _IMGUIBASE_H_
#define _IMGUIBASE_H_

#include "eventhandler.h"

namespace ysl
{
	namespace imgui
	{
		class ImGuiInterface
		{
		public:
			ImGuiInterface() = default;
			virtual void Draw() = 0;
			virtual ~ImGuiInterface() = default;
		//protected:
		//	virtual void MouseMoveEvent(MouseEvent *);
		//	virtual void MousePressEvent(MouseEvent *);
		//	virtual void MouseReleaseEvent(MouseEvent *);
		//	virtual void WindowResizeEvent(ResizeEvent *);
		};
	}
}



#endif

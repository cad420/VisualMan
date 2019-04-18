

#ifndef _APPLICATION_H_
#define _APPLICATION_H_
#include "../utility/utils.h"
#include "event.h"


#define App(AppClass) (dynamic_cast<AppClass*>(Application::Instance()))

class MouseEvent;

namespace ysl
{
	namespace app
	{
		class Application
		{
			public:
				Application(int argc, char ** argv);
				DISABLE_COPY(Application);
				DISABLE_MOVE(Application);
				virtual int Exec() = 0;
				virtual ~Application();
				static Application * Instance();
			protected:
				virtual void MousePressEvent(MouseEvent *e) = 0;
				virtual void MouseReleaseEvent(MouseEvent *e) = 0;
				virtual void MouseMoveEvent(MouseEvent *e) = 0;
				virtual void WindowResizeEvent(ResizeEvent * event)=0;
				virtual void MouseWheelEvent(WheelEvent* e) = 0;
				static Application * app;
		};
	}
}

#endif

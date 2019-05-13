
#ifndef _APPLET_H_
#define _APPLET_H_
#include "eventinterface.h"
#include "abstrascenemanager.h"

namespace ysl
{
	namespace gpu
	{
		class RenderAssembly :public IEventListener
		{
		public:

			// IEventListener interfaces
			RenderAssembly() = default;
			void InitEvent()override{}
			void DestroyEvent() override{}
			void UpdateEvent()override;
			void MousePressEvent(MouseButton button, int xpos, int ypos) override;
			void MouseReleaseEvent(MouseButton button, int xpos, int ypos) override;
			void KeyPressEvent(KeyButton key) override{}
			void KeyReleaseEvent(KeyButton key) override{}
			void ResizeEvent(int w, int h) override;
			
		protected:
			std::vector<Ref(AbstraSceneManager)> sceneManagers;

		};
	}
}

#endif
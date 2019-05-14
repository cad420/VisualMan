
#ifndef _ASSEMBLY_H_
#define _ASSEMBLY_H_
#include "eventinterface.h"
#include <vector>

namespace ysl
{
	namespace gpu
	{
		class AbstraSceneManager;


		class GRAPHICS_EXPORT_IMPORT Assembly :public IEventListener
		{
		public:

			// IEventListener interfaces
			Assembly() = default;
			void InitEvent() override;
			void DestroyEvent() override;
			void UpdateEvent()override;
			void MousePressEvent(EMouseButton button, int xpos, int ypos) override;
			void MouseReleaseEvent(EMouseButton button, int xpos, int ypos) override;
			void MouseMoveEvent(EMouseButton button, int xpos, int ypos) override;
			void MouseWheelEvent(int ydegree, int xdegree) override;
			void KeyPressEvent(EKeyButton key) override;
			void KeyReleaseEvent(EKeyButton key) override;
			void ResizeEvent(int w, int h) override;

		protected:
			std::vector<Ref<AbstraSceneManager>> sceneManagers;
		};
	}
}

#endif
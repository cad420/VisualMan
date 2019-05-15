
#ifndef _ASSEMBLY_H_
#define _ASSEMBLY_H_

#include "eventinterface.h"
#include "renderstudio.h"

namespace ysl
{
	namespace graphics
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

			virtual void UpdateScene(){}


			Ref<RenderStudio> Rendering()const { return studio; }

		protected:
			Ref<RenderStudio> studio;
			Ref<Camera> camera;

			double FPS;
		};
	}
}

#endif
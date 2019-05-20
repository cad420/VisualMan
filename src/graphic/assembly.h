
#ifndef _ASSEMBLY_H_
#define _ASSEMBLY_H_

#include "eventinterface.h"
#include "renderstudio.h"

namespace ysl
{
	namespace vpl
	{
		class AbstraSceneManager;

		class GRAPHICS_EXPORT_IMPORT Assembly :public IEventListener
		{
		public:

			void InitDefault();

			// IEventListener interfaces
			Assembly();
			void InitEvent() override{}
			void DestroyEvent() override;
			void UpdateEvent()override;
			void AddedEvent(RenderContext* context) override;
			void DeletedEvent(RenderContext* context) override;
			void MousePressEvent(EMouseButton button, int xpos, int ypos) override;
			void MouseReleaseEvent(EMouseButton button, int xpos, int ypos) override;
			void MouseMoveEvent(EMouseButton button, int xpos, int ypos) override;
			void MouseWheelEvent(int ydegree, int xdegree) override;
			void KeyPressEvent(EKeyButton key) override;
			void KeyReleaseEvent(EKeyButton key) override;
			void ResizeEvent(int w, int h) override;

			virtual void UpdateScene(){}

			Ref<Frame> Rendering()const { return frame; }

		protected:
			Ref<Frame> frame;
			Ref<CameraManipulator> manipulator;
			double FPS;
		};
	}
}

#endif
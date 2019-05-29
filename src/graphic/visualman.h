
#ifndef _VISUALMAN_H_
#define _VISUALMAN_H_

#include "eventinterface.h"
#include "renderstudio.h"

namespace ysl
{
	namespace vm
	{
		class AbstraSceneManager;

		class GRAPHICS_EXPORT_IMPORT VisualMan :public IEventListener
		{
		public:
			void InitDefault();
			// IEventListener interfaces
			VisualMan();

			void InitEvent() override{}

			void DestroyEvent() override;

			void UpdateEvent() override;

			void AddedEvent(RenderContext* context) override;

			void DeletedEvent(RenderContext* context) override;

			void MousePressEvent(MouseButton button, int xpos, int ypos) override;

			void MouseReleaseEvent(MouseButton button, int xpos, int ypos) override;

			void MouseMoveEvent(MouseButton button, int xpos, int ypos) override;

			void MouseWheelEvent(int ydegree, int xdegree) override;

			void KeyPressEvent(KeyButton key) override;

			void KeyReleaseEvent(KeyButton key) override;

			void ResizeEvent(int w, int h) override;

			virtual void UpdateScene(){}

			Ref<Aggregate> GetAggregate()const { return aggregate; }

			void SetAggregation(Ref<Aggregate> aggregate) { this->aggregate = std::move(aggregate); }

			void BindCameraEvent(Ref<Camera> camera);

		protected:
			Ref<Aggregate> aggregate;
			Ref<CameraManipulator> manipulator;
			double FPS;
		};
	}
}

#endif
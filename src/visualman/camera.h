
#ifndef _CAMERA_H_
#define _CAMERA_H_
#include "graphictype.h"
#include "../common/geometry.h"
#include "../common/transformation.h"
#include "eventinterface.h"
#include "viewport.h"

namespace ysl
{
	namespace vm
	{

		class Viewport;

		class Camera_Impl
		{
			static constexpr float YAW = -90.0f;
			static constexpr float PITCH = 0.0f;
			static constexpr float SPEED = 2.5f;
			static constexpr float SENSITIVITY = 0.1f;
			static constexpr float ZOOM = 45.0f;

			// Camera Attributes
			Point3f m_position;
			Vector3f m_front;
			Vector3f m_up;
			Vector3f m_right;
			Vector3f m_worldUp;
			Point3f m_center;

			// Camera options
			float m_movementSpeed;
			float m_mouseSensitivity;
			float m_zoom;

		public:

			// Constructor with vectors
			Camera_Impl(const ysl::Point3f& position = { 0.0f, 0.0f, 0.0f }, ysl::Vector3f up = { 0.0f, 1.0f, 0.0f },
				const ysl::Point3f& center = { 0, 0, 0 });

			Vector3f front()const { return m_front; }
			Vector3f right()const { return m_right; }
			Vector3f up()const { return m_up; }

			// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
			Transform view()const
			{
				ysl::Transform vi;
				vi.SetLookAt(m_position, m_position + m_front, m_up);
				return vi;
			}

			Point3f position()const { return m_position; }

			Point3f center()const { return m_center; }

			void setCenter(const ysl::Point3f& center);

			void movement(const ysl::Vector3f& direction, float deltaTime);

			void rotation(float xoffset, float yoffset);

			void processMouseScroll(float yoffset);

		private:

			void updateCameraVectors(const ysl::Vector3f& axis, double theta);
		};


		class GRAPHICS_EXPORT_IMPORT Camera
		{
		public:
			Camera(int w = 800,int h = 600,const Point3f& position = { 0.0f, 0.0f, 5.0f }, 
				Vector3f up = { 0.0f, 1.0f, 0.0f },
				const Point3f& center = { 0, 0, 0 }):focusCamera(position,up,center)
			{
				viewport = MakeRef<Viewport>(w, h);
			}

			Transform ViewMatrix()const { return focusCamera.view(); }
			void SetProjectionMatrix(const Transform & projection) { this->projMatrix = projection; }
			const Transform &ProjectionMatrix()const { return projMatrix; }
			Transform ProjectViewMatrix()const { return projMatrix * ViewMatrix(); }
			Point3f Position()const { return focusCamera.position(); }
			void Ratation(float xoffset, float yoffset) { focusCamera.rotation(xoffset, yoffset); }

			void SetViewport(Ref<Viewport> vp) { viewport = std::move(vp); }

			Ref<Viewport> GetViewport() { return viewport; }

			Ref<const Viewport> GetViewport()const { return viewport; }

			//Ref<Viewport> CreateGetViewport() { return viewport ? viewport : viewport = MakeRef<Viewport>(800,600); }

			Vec3f Up()const { return focusCamera.up(); }
			Vec3f Front()const { return focusCamera.front(); }
			Vec3f Right()const { return focusCamera.right(); }
			void Movement(const Vec3f& dir, float deltaTime) { focusCamera.movement(dir, deltaTime); }

		private:
			Camera_Impl focusCamera;
			Transform projMatrix;
			Ref<Viewport> viewport;
		};

		class GRAPHICS_EXPORT_IMPORT CameraManipulator:public IEventListener
		{
		public:
			CameraManipulator(Ref<Camera> camera = nullptr):camera(std::move(camera)){}

			void SetCamera(Ref<Camera> camera) { this->camera = std::move(camera); }

			Ref<Camera> GetCamera() { return camera; }

			Ref<const Camera> GetCamera()const { return camera; }

			void InitEvent()override{}

			void DestroyEvent()override{}

			/**
			 * \brief Change the viewport and the projection matrix
			 */
			void ResizeEvent(int w, int h)override;

			void UpdateEvent() override{}

			void AddedEvent(RenderContext * context) override;

			void DeletedEvent(RenderContext * context) override;

			void MousePressEvent(MouseButton button, int xpos, int ypos) override;

			void MouseMoveEvent(MouseButton button, int xpos, int ypos) override;

			void MouseReleaseEvent(MouseButton button, int xpos, int ypos) override{};

			void MouseWheelEvent(int ydegree, int xdegree) override{}

			void KeyPressEvent(KeyButton key) override{}

			void KeyReleaseEvent(KeyButton key) override{}

		private:
			Ref<Camera> camera;
			Vec2i lastMousePos;

		};
	}
}

#endif
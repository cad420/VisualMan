
#ifndef _CAMERA_H_
#define _CAMERA_H_
#include "graphictype.h"
#include "../mathematics/geometry.h"
#include "../mathematics/transformation.h"

namespace ysl
{
	namespace graphics
	{

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
			Camera(const Point3f& position = { 0.0f, 0.0f, 0.0f }, Vector3f up = { 0.0f, 1.0f, 0.0f },
				const Point3f& center = { 0, 0, 0 }):focusCamera(position,up,center)
			{
				
			}

			Transform ViewMatrix()const { return focusCamera.view(); }
			void SetProjectionMatrix(const Transform & projection) { this->projMatrix = projection; }
			const Transform &ProjectionMatrix()const { return projMatrix; }
			Transform ProjectViewMatrix()const { return projMatrix * ViewMatrix(); }
			Point3f Position()const { return focusCamera.position(); }
			void Ratation(float xoffset, float yoffset) { focusCamera.rotation(xoffset, yoffset); }


		private:
			Camera_Impl focusCamera;
			Transform projMatrix;
		};
	}
}

#endif
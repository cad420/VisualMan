#ifndef CAMERA_H
#define CAMERA_H

#include "geometry.h"
#include "transformation.h"


class FocusCamera
{
	static constexpr float YAW = -90.0f;
	static constexpr float PITCH = 0.0f;
	static constexpr float SPEED = 2.5f;
	static constexpr float SENSITIVITY = 0.1f;
	static constexpr float ZOOM = 45.0f;

	// Camera Attributes
	ysl::Point3f m_position;
	ysl::Vector3f m_front;
	ysl::Vector3f m_up;
	ysl::Vector3f m_right;
	ysl::Vector3f m_worldUp;
	ysl::Point3f m_center;

	// Camera options
	float m_movementSpeed;
	float m_mouseSensitivity;
	float m_zoom;

public:

	// Constructor with vectors
	FocusCamera(const ysl::Point3f& position = {0.0f, 0.0f, 0.0f}, ysl::Vector3f up = {0.0f, 1.0f, 0.0f},
	            const ysl::Point3f& center = {0, 0, 0});

	ysl::Vector3f front()const { return m_front; }
	ysl::Vector3f right()const { return m_right; }
	ysl::Vector3f up()const { return m_up; }

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	ysl::Transform view()const
	{
		ysl::Transform vi;
		vi.SetLookAt(m_position, m_position + m_front, m_up);
		return vi;
	}

	ysl::Point3f position()const { return m_position; }

	ysl::Point3f center()const { return m_center; }

	void setCenter(const ysl::Point3f& center);

	void movement(const ysl::Vector3f& direction, float deltaTime);

	void rotation(float xoffset, float yoffset);

	void processMouseScroll(float yoffset);

private:

	void updateCameraVectors(const ysl::Vector3f& axis, double theta);
};



#endif // CAMERA_H

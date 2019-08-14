#include "frustum.h"
#include <transformation.h>
//#include "camera.h"

namespace ysl
{
	Frustum::Frustum(const vm::Camera& camera)
	{
		Init(camera);
	}

	std::vector<Point2f> Frustum::GetFrustumLines() const
	{
		return std::vector<Point2f>();
	}

	void Frustum::Init(const vm::Camera & camera)
	{
		const auto fov = camera.GetFov();
		const auto pos = camera.GetPosition();
		const auto nearPlane = camera.GetNearPlane();
		const auto farPlane = camera.GetFarPlane();
		const auto direction = camera.GetFront().Normalized();
		const auto right = camera.Right().Normalized();
		const auto up = camera.Up().Normalized();
		const auto aspectRatio = camera.GetAspectRatio();

		planes[0] = Plane(direction, pos + direction * nearPlane);
		planes[1] = Plane(direction, pos + direction * farPlane);
		planes[2] = Plane(Rotate(up, fov / 2.f) * (-right), pos);
		planes[3] = Plane(Rotate(up, -fov / 2.f) * right, pos);
		planes[4] = Plane(Rotate(right, aspectRatio * fov / 2.f) * up, pos);
		planes[5] = Plane(Rotate(right, -aspectRatio * fov / 2.f) * (-up), pos);

	}
	
}
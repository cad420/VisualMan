
#include "pbrcamera.h"

namespace ysl
{
	ProjectiveCamera::ProjectiveCamera(const Transform& cameraToWorld,
		const Transform & cameraToScreen ,
		const Bound2f& screenBound, 
		Film* film):Camera(cameraToWorld,film),CameraToScreen(cameraToScreen)
	{
		ScreenToRaster = Scale(film->resolution.x, film->resolution.y, 1.0) *
			Scale(1 / (screenBound.max.x - screenBound.min.x), 1 / (screenBound.max.y - screenBound.min.y), 1.0) *
			Translate(-screenBound.min.x, -screenBound.max.y, 0);
		RasterToScreen = ScreenToRaster.Inversed();
		RasterToCamera = cameraToScreen.Inversed() * RasterToScreen;
	}

	OrthographicCamera::OrthographicCamera(const Transform& cameraToWorld, 
		const Bound2f& screenSize, 
		Film* film):ProjectiveCamera(cameraToWorld,Orthographic(0,1),screenSize,film)
	{

	}

	Float OrthographicCamera::GenerateRay(const CameraSample& sample, Ray* ray) const
	{
		const auto pFilm = Point3f(pFilm.x, pFilm.y, 0);
		const auto pCamera = RasterToCamera * pFilm;
		*ray = Ray(Vector3f(0, 0, 1), pCamera);

		///TODO:: Add ray time and medium filed later

		*ray = CameraToWorld * (*ray);
		return 1;
	}

	PerspectiveCamera::PerspectiveCamera(const Transform& cameraToWorld, 
		const Bound2f& screenSize,Float fov, 
		Film* film):ProjectiveCamera(cameraToWorld,Perspective(fov,0.0001,1000.f),screenSize,film)
	{

	}

	Float PerspectiveCamera::GenerateRay(const CameraSample& sample, Ray* ray) const
	{
		const auto pFilm = Point3f(sample.pFilm.x,sample.pFilm.y,0);
		const auto pCamera = RasterToCamera * pFilm;
		*ray = Ray(Vector3f(pCamera).Normalized(),{0,0,0});

		*ray = CameraToWorld * (*ray);
		return 1;
	}
}

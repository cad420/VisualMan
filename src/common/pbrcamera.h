
#ifndef _PBRCAMERA_H_
#define _PBRCAMERA_H_
#include "geometry.h"
#include "transformation.h"
#include "film.h"

namespace ysl
{

	struct CameraSample
	{
		Point2f pFilm;
		Point2f pLens;
		Float time;
	};

	class Camera
	{
	public:
		Camera(const Transform & cameraToWorld,Film * film):CameraToWorld(cameraToWorld),film(film){}
		virtual Float GenerateRay(const CameraSample & sample, Ray * ray)const = 0;
		virtual ~Camera();
		// Film
		// Medium
		Transform CameraToWorld;			// It transform the ray from camera space to world space, i.e. The inverse of LookAt transform
		Film * film;
	};


	class ProjectiveCamera:public Camera
	{
	public:
		ProjectiveCamera(const Transform & cameraToWorld,const Transform & cameraToScreen,const Bound2f & screenSize,Film * film);
	protected:
		Transform ScreenToRaster; // 
		Transform RasterToScreen;
		Transform CameraToScreen;
		Transform RasterToCamera; // rasterToCamera transform the point on the canvas to the space where the camera is (camera space)
	};

	class OrthographicCamera:public ProjectiveCamera
	{
	public:
		OrthographicCamera(const Transform & cameraToWorld,const Bound2f & screenSize,Film * film);
		Float GenerateRay(const CameraSample& sample, Ray* ray) const override;
	};

	class PerspectiveCamera:public ProjectiveCamera
	{
	public:
		PerspectiveCamera(const Transform & cameraToWorld,const Bound2f & screenSize,Float fov,Film * film);
		Float GenerateRay(const CameraSample& sample, Ray* ray) const override;
	};

}

#endif

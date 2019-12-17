
#ifndef _CAMERA_H_
#define _CAMERA_H_
#include <VMat/geometry.h>
#include <VMat/transformation.h>

#include "graphictype.h"
#include "eventinterface.h"
#include "viewport.h"
#include "abstraarray.h"
#include <VMUtils/json_binding.hpp>

namespace vm
{
class Viewport;

struct ViewMatrixJSONStruct : json::Serializable<ViewMatrixJSONStruct>
{
	VM_JSON_FIELD( std::vector<float>, pos );
	VM_JSON_FIELD( std::vector<float>, up );
	VM_JSON_FIELD( std::vector<float>, front );
};

struct PerspMatrixJSONStruct : json::Serializable<PerspMatrixJSONStruct>
{
	VM_JSON_FIELD( float, fov );
	VM_JSON_FIELD( float, nearPlane );
	VM_JSON_FIELD( float, farPlane );
	VM_JSON_FIELD( float, aspectRatio );
};

struct CameraJSONStruct : json::Serializable<CameraJSONStruct>
{
	VM_JSON_FIELD( ViewMatrixJSONStruct, viewMatrix );
	VM_JSON_FIELD( PerspMatrixJSONStruct, perspectiveMatrix );
};

class VISUALMAN_EXPORT_IMPORT ViewMatrixWrapper
{
	static constexpr float YAW = -90.0f;
	static constexpr float PITCH = 0.0f;
	static constexpr float SPEED = 1.0f;
	static constexpr float SENSITIVITY = .1f;
	static constexpr float ZOOM = 45.0f;

	// Camera Attributes
	Point3f m_position;
	Vector3f m_front;
	Vector3f m_up;
	Vector3f m_right;  // Redundant
	Vector3f m_worldUp;
	//Point3f m_center;

	// Camera options
	float m_movementSpeed;
	float m_mouseSensitivity;
	float m_zoom;

public:
	// Constructor with vectors
	ViewMatrixWrapper( const Point3f &position = { 0.0f, 0.0f, 0.0f }, Vector3f up = { 0.0f, 1.0f, 0.0f },
					   const Point3f &center = { 0, 0, 0 } );
	ViewMatrixWrapper( const Point3f &position, Vector3f up,
					   const Vector3f &front );

	Vector3f GetFront() const { return m_front; }
	void SetFront( const Vector3f &front ) { m_front = front.Normalized(); }
	Vector3f GetRight() const { return m_right; }
	Vector3f GetUp() const { return m_up; }
	void UpdateCamera( const Point3f &position, Vector3f worlUp,
					   const Point3f &center );
	void ViewMatrixWrapper::UpdateCamera( const Point3f &position, Vector3f worldUp, const Vec3f &front );
	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	Transform GetViewMatrix() const;
	Point3f GetPosition() const { return m_position; }
	void SetPosition( const Point3f &pos );
	//Point3f GetCenter() const { return m_center; }
	void SetCenter( const Point3f &center );
	void Move( const Vector3f &direction, float deltaTime );
	void Rotate( float xOffset, float yOffset, const Point3f &center );
	void ProcessMouseScroll( float yOffset );
	void RotateCamera( const Vector3f &axis, double theta, const Point3f &center );

private:
};

class VISUALMAN_EXPORT_IMPORT Camera
{
public:
	Camera( const Point3f &position = { 0.0f, 0.0f, 5.0f },
			Vector3f up = { 0.0f, 1.0f, 0.0f },
			const Point3f &center = { 0, 0, 0 } )  //:viewMatrixWrapper(position,up,center)
	  ;

	Transform ViewMatrix() const { return viewMatrixWrapper->GetViewMatrix(); }

	VMRef<ViewMatrixWrapper> GetViewMatrixWrapper() { return viewMatrixWrapper; }

	VMRef<const ViewMatrixWrapper> GetViewMatrixWrapper() const { return viewMatrixWrapper; }

	void SetViewMatrixWrapper( VMRef<ViewMatrixWrapper> viewMatrixWrapper ) { this->viewMatrixWrapper = std::move( viewMatrixWrapper ); }

	void SetProjectionMatrix( const Transform &projection );

	const Transform &ProjectionMatrix() const;

	Transform ProjectViewMatrix() const { return ( *projMatrix ) * ViewMatrix(); }

	VMRef<Transform> GetPerspectiveMatrix() { return projMatrix; }

	VMRef<const Transform> GetPerspectiveMatrix() const { return projMatrix; }

	void SetPerspectiveMatrix( VMRef<Transform> persp ) { projMatrix = std::move( persp ); }

	Point3f GetPosition() const { return viewMatrixWrapper->GetPosition(); }

	void Rotation( float xoffset, float yoffset,const Point3f & center ) { viewMatrixWrapper->Rotate( xoffset, yoffset, center); }

	Vector3f GetFront() const { return viewMatrixWrapper->GetFront(); }

	void SetFront( const Vector3f &front ) { viewMatrixWrapper->SetFront( front ); }

	void SetViewport( VMRef<Viewport> vp ) { viewport = std::move( vp ); }

	Vector3f GetUp() const { return viewMatrixWrapper->GetUp(); }


	void SetCenter( const Point3f &center ) { viewMatrixWrapper->SetCenter( center ); }

	void SetCamera( VMRef<ViewMatrixWrapper> viewMatrixWrapper, VMRef<Transform> projMatrix );

	void SetCamera( const Point3f &position, Vector3f worlUp,
					const Point3f &center, float nearPlane, float farPlane, float aspectRatio, float fov );

	void SetCamera( const Point3f &position, Vector3f worlUp,
					const Vector3f &front, float nearPlane, float farPlane, float aspectRatio, float fov );


	float GetFov() const { return fov; }

	void SetFov( float fov )
	{
		this->fov = Clamp( fov, 1.f, 89.f );
		UpdateProjMatrix();
	}

	float GetNearPlane() const { return nearPlan; }

	void SetNearPlane( float np )
	{
		nearPlan = np;
		UpdateProjMatrix();
	}

	float GetFarPlane() const { return farPlan; }

	void SetFarPlane( float fp )
	{
		farPlan = fp;
		UpdateProjMatrix();
	}

	float GetAspectRatio() const { return aspectRatio; }

	void SetAspectRation( float aspect ) { aspectRatio = aspect; }

	VMRef<Viewport> GetViewport() { return viewport; }

	VMRef<const Viewport> GetViewport() const { return viewport; }

	Vec3f Up() const { return viewMatrixWrapper->GetUp(); }

	Vec3f Right() const { return viewMatrixWrapper->GetRight(); }

	void Movement( const Vec3f &dir, float deltaTime ) { viewMatrixWrapper->Move( dir, deltaTime ); }

	std::vector<Point3f> GetFrustumLines() const;

private:
	void UpdateProjMatrix() { projMatrix->SetGLPerspective( fov, aspectRatio, nearPlan, farPlan ); }
	VMRef<ViewMatrixWrapper> viewMatrixWrapper;
	VMRef<Transform> projMatrix;
	VMRef<Viewport> viewport;
	float fov = 60;
	float aspectRatio = 1024.0 / 768.0;
	float nearPlan = 0.01;
	float farPlan = 1000;
};

VISUALMAN_EXPORT_IMPORT VMRef<Camera> CreateCamera( const std::string &jsonFileName );
VISUALMAN_EXPORT_IMPORT void ConfigCamera( Camera *camera, const std::string &jsonFileName );
VISUALMAN_EXPORT_IMPORT bool SaveCameraAsJson( VMRef<Camera> camera, const std::string &jsonFileName );

class VISUALMAN_EXPORT_IMPORT CameraManipulator : public IEventListener
{
public:
	CameraManipulator( VMRef<Camera> camera = nullptr ) :
	  camera( std::move( camera ) ) {}

	void SetCamera( VMRef<Camera> camera ) { this->camera = std::move( camera ); }

	VMRef<Camera> GetCamera() { return camera; }

	VMRef<const Camera> GetCamera() const { return camera; }

	void InitEvent() override {}

	void DestroyEvent() override {}

	/**
			 * \brief Change the viewport and the projection matrix
			 */
	void ResizeEvent( int w, int h ) override;

	void UpdateEvent() override {}

	void AddedEvent( RenderContext *context ) override;

	void DeletedEvent( RenderContext *context ) override;

	void MousePressEvent( MouseButton button, int xpos, int ypos ) override;

	void MouseMoveEvent( MouseButton button, int xpos, int ypos ) override;

	void MouseReleaseEvent( MouseButton button, int xpos, int ypos ) override{};

	void MouseWheelEvent( int ydegree, int xdegree ) override {}

	void FileDropEvent( const std::vector<std::string> &fileNames ) override{};

	void KeyPressEvent( KeyButton key ) override;

	void KeyReleaseEvent( KeyButton key ) override {}

	void SetFPSCamera( bool enable ) { m_fpsCamera = enable; }

	bool IsFPSCamera() const { return m_fpsCamera; }

private:
	VMRef<Camera> camera;
	Vec2i lastMousePos;
	bool m_fpsCamera = true;
};


}  // namespace vm

#endif

#ifndef _CAMERA_H_
#define _CAMERA_H_
#include <geometry.h>
#include <transformation.h>

#include "graphictype.h"
#include "eventinterface.h"
#include "viewport.h"
#include "abstraarray.h"

namespace ysl
{
	namespace vm
	{

		class Viewport;

		class ViewMatrixWrapper
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
			Vector3f m_right;		// Redundant
			Vector3f m_worldUp;
			Point3f m_center;

			// Camera options
			float m_movementSpeed;
			float m_mouseSensitivity;
			float m_zoom;

		public:

			// Constructor with vectors
			ViewMatrixWrapper(const ysl::Point3f& position = { 0.0f, 0.0f, 0.0f }, ysl::Vector3f up = { 0.0f, 1.0f, 0.0f },
				const ysl::Point3f& center = { 0, 0, 0 });

			Vector3f GetFront()const { return m_front; }
			void SetFront(const Vector3f & front) { m_front = front.Normalized();}
			Vector3f GetRight()const { return m_right; }
			Vector3f GetUp()const { return m_up; }
			void UpdateCamera(const ysl::Point3f& position , ysl::Vector3f worlUp ,
				const ysl::Point3f& center );
			// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
			Transform GetViewMatrix() const;
			Point3f GetPosition()const { return m_position; }
			Point3f GetCenter()const { return m_center; }
			void SetCenter(const ysl::Point3f& center);
			void Move(const ysl::Vector3f& direction, float deltaTime);
			void Rotate(float xOffset, float yOffset);
			void ProcessMouseScroll(float yOffset);
		private:
			void UpdateCameraVectors(const ysl::Vector3f& axis, double theta);
		};



		class VISUALMAN_EXPORT_IMPORT Camera
		{
		public:
			Camera(const Point3f& position = {0.0f, 0.0f, 5.0f},
			       Vector3f up = {0.0f, 1.0f, 0.0f},
			       const Point3f& center = {0, 0, 0}) //:viewMatrixWrapper(position,up,center)
			;

			Transform ViewMatrix()const { return viewMatrixWrapper->GetViewMatrix(); }

			Ref<ViewMatrixWrapper> GetViewMatrixWrapper() { return viewMatrixWrapper; }

			Ref<const ViewMatrixWrapper> GetViewMatrixWrapper()const { return viewMatrixWrapper; }

			void SetViewMatrixWrapper(Ref<ViewMatrixWrapper> viewMatrixWrapper) { this->viewMatrixWrapper = std::move(viewMatrixWrapper); }

			void SetProjectionMatrix(const Transform& projection);

			const Transform& ProjectionMatrix() const;
			
			Transform ProjectViewMatrix()const { return (*projMatrix)*ViewMatrix(); }

			Ref<Transform> GetPerspectiveMatrix() { return projMatrix; }

			Ref<const Transform> GetPerspectiveMatrix()const { return projMatrix; }

			void SetPerspectiveMatrix(Ref<Transform> persp) { projMatrix = std::move(persp); }


			Point3f GetPosition()const { return viewMatrixWrapper->GetPosition(); }

			void Rotation(float xoffset, float yoffset) { viewMatrixWrapper->Rotate(xoffset, yoffset); }

			Vector3f GetFront()const { return viewMatrixWrapper->GetFront(); }

			void SetViewport(Ref<Viewport> vp) { viewport = std::move(vp); }

			Vector3f GetUp()const { return viewMatrixWrapper->GetUp(); }

			Point3f GetCenter()const { return viewMatrixWrapper->GetCenter(); }

			void SetCamera(Ref<ViewMatrixWrapper> viewMatrixWrapper, Ref<Transform> projMatrix);

			void SetCamera(const ysl::Point3f& position, ysl::Vector3f worlUp,
			               const ysl::Point3f& center, float nearPlane, float farPlane, float aspectRatio, float fov);

			float GetFov()const { return fov; }

			void SetFov(float fov) { this->fov = Clamp(fov,1.f,89.f); UpdateProjMatrix(); }

			float GetNearPlane()const { return nearPlan; }

			void SetNearPlane(float np) { nearPlan = np; UpdateProjMatrix(); }

			float GetFarPlane()const { return farPlan; }

			void SetFarPlane(float fp) { farPlan = fp; UpdateProjMatrix(); }

			float GetAspectRatio()const { return aspectRatio; }

			void SetAspectRation(float aspect) { aspectRatio = aspect; }

			Ref<Viewport> GetViewport() { return viewport; }

			Ref<const Viewport> GetViewport()const { return viewport; }

			Vec3f Up()const { return viewMatrixWrapper->GetUp(); }

			Vec3f Right()const { return viewMatrixWrapper->GetRight(); }

			void Movement(const Vec3f& dir, float deltaTime) { viewMatrixWrapper->Move(dir, deltaTime); }

			Ref<ArrayFloat3> GetFrustumLines() const;

		private:
			void UpdateProjMatrix(){ projMatrix->SetGLPerspective(fov, aspectRatio, nearPlan, farPlan);}
			Ref<ViewMatrixWrapper> viewMatrixWrapper;
			Ref<Transform> projMatrix;
			Ref<Viewport> viewport;
			float fov = 60;
			float aspectRatio = 1024.0 / 768.0;
			float nearPlan = 0.01;
			float farPlan = 1000;
		};

		VISUALMAN_EXPORT_IMPORT Ref<Camera> CreateCamera(const std::string & jsonFileName);
		VISUALMAN_EXPORT_IMPORT void ConfigCamera(Camera * camera,const std::string & jsonFileName);
		VISUALMAN_EXPORT_IMPORT bool SaveCameraAsJson(Ref<Camera> camera,const std::string & jsonFileName);

		class VISUALMAN_EXPORT_IMPORT CameraManipulator:public IEventListener
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

			void FileDropEvent(const std::vector<std::string>& fileNames) override {};

			void KeyPressEvent(KeyButton key) override{}

			void KeyReleaseEvent(KeyButton key) override{}

		private:
			Ref<Camera> camera;
			Vec2i lastMousePos;
		};
	}
}

#endif
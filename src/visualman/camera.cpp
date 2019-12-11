
#include "camera.h"
#include "viewport.h"
#include "rendercontext.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/pointer.h"
#include "rapidjson/writer.h"

namespace ysl
{
namespace vm
{
ViewMatrixWrapper::ViewMatrixWrapper( const ysl::Point3f &position, ysl::Vector3f worldUp, const ysl::Point3f &center ) :
  m_position( position ),
  m_front( center - position ),
  m_worldUp( worldUp ),
  m_movementSpeed( SPEED ),
  m_mouseSensitivity( SENSITIVITY ),
  m_center( center ),
  m_zoom( ZOOM )
{
	m_right = ysl::Vector3f::Cross( m_front, m_worldUp ).Normalized();
	m_up = ysl::Vector3f::Cross( m_right, m_front ).Normalized();
	//updateCameraVectors(QVector3D(0,1,0),QVector3D(0,0,0),0);
}

void ViewMatrixWrapper::UpdateCamera( const ysl::Point3f &position, ysl::Vector3f worldUp, const ysl::Point3f &center )
{
	m_position = position;
	m_front = center - position;
	m_worldUp = worldUp;
	m_right = ysl::Vector3f::Cross( m_front, m_worldUp ).Normalized();
	m_up = ysl::Vector3f::Cross( m_right, m_front ).Normalized();
	m_center = center;
}

Transform ViewMatrixWrapper::GetViewMatrix() const
{
	ysl::Transform vi;
	vi.SetLookAt( m_position, m_position + m_front, m_up );
	return vi;
}

void ViewMatrixWrapper::SetPosition( const Point3f &pos )
{
	m_position = pos;
}

void ViewMatrixWrapper::SetCenter( const ysl::Point3f &center )
{
	m_center = center;
	m_front = ( m_center - m_position ).Normalized();
	m_right = ysl::Vector3f::Cross( m_front, m_worldUp ).Normalized();
	m_up = ysl::Vector3f::Cross( m_right, m_front ).Normalized();
}

void ViewMatrixWrapper::Move( const ysl::Vector3f &direction, float deltaTime )
{
	const auto velocity = m_movementSpeed * direction * deltaTime;
	m_position += velocity;
}

void ViewMatrixWrapper::Rotate( float xoffset, float yoffset )
{
	xoffset *= m_mouseSensitivity;
	yoffset *= m_mouseSensitivity;
	const auto theta = 4.0 * ( std::fabs( xoffset ) + std::fabs( yoffset ) );
	const auto v = ( ( m_right * xoffset ) + ( m_up * yoffset ) );
	const auto axis = ysl::Vector3f::Cross( v, -m_front ).Normalized();
	RotateCamera( axis, theta );
}

void ViewMatrixWrapper::ProcessMouseScroll( float yoffset )
{
	if ( m_zoom >= 1.0f && m_zoom <= 45.0f )
		m_zoom -= yoffset;
	if ( m_zoom <= 1.0f )
		m_zoom = 1.0f;
	if ( m_zoom >= 45.0f )
		m_zoom = 45.0f;
}

void ViewMatrixWrapper::RotateCamera( const ysl::Vector3f &axis, double theta )
{
	ysl::Transform rotation;
	rotation.SetRotate( axis, theta );
	ysl::Transform translation;
	translation.SetTranslate( -m_center.x, -m_center.y, -m_center.z );
	m_position = translation.Inversed() * ( rotation * ( translation * m_position ) );
	m_front = ( rotation * m_front.Normalized() );
	m_up = ( rotation * m_up.Normalized() );
	m_right = ysl::Vector3f::Cross( m_front, m_up );
	m_up = ysl::Vector3f::Cross( m_right, m_front );
	m_front.Normalize();
	m_right.Normalize();
	m_up.Normalize();
}

Camera::Camera( const Point3f &position, Vector3f up, const Point3f &center )
{
	viewMatrixWrapper = MakeRef<ViewMatrixWrapper>( position, up, center );
	viewport = MakeRef<Viewport>();
	projMatrix = MakeRef<Transform>();
	projMatrix->SetGLPerspective( fov, aspectRatio, nearPlan, farPlan );
}

void Camera::SetProjectionMatrix( const Transform &projection )
{
	*( this->projMatrix ) = projection;
}

const Transform &Camera::ProjectionMatrix() const
{
	return *projMatrix;
}

void Camera::SetCamera( Ref<ViewMatrixWrapper> viewMatrixWrapper, Ref<Transform> projMatrix )
{
	if ( viewMatrixWrapper )
		this->viewMatrixWrapper = viewMatrixWrapper;
	if ( projMatrix )
		this->projMatrix = projMatrix;
}

void Camera::SetCamera( const ysl::Point3f &position, ysl::Vector3f worlUp, const ysl::Point3f &center,
						float nearPlane, float farPlane, float aspectRatio, float fov )
{
	viewMatrixWrapper->UpdateCamera( position, worlUp, center );

	this->nearPlan = nearPlane;
	this->farPlan = farPlane;
	this->aspectRatio = aspectRatio;
	this->fov = fov;

	Transform perspectiveMatrix;
	perspectiveMatrix.SetGLPerspective( fov, aspectRatio, nearPlane, farPlane );
	SetProjectionMatrix( perspectiveMatrix );
}

std::vector<Point3f> Camera::GetFrustumLines() const
{
	/*
			 *
			 * 7------6
			 * |\	 /|
			 * | 3--2 |
			 * | |	| |
			 * | 0--1 |
			 * 4------5
			 *
			 * Frustum
			 */

	const auto pos = GetPosition();
	const auto direction = GetFront().Normalized();
	const auto right = Right().Normalized();
	const auto up = Up().Normalized();
	const auto farPlane = ( GetFarPlane() + GetNearPlane() ) / 2.0;
	const auto nearPlane = ( GetNearPlane() );
	const auto tanfov2 = 2.0 * std::tan( GetFov() * Pi / 180 / 2.0 );
	const auto atanfov2 = GetAspectRatio() * tanfov2;
	const auto nearCenter = pos + direction * nearPlane;
	const auto farCenter = pos + direction * farPlane;
	const auto nearHalfHeight = tanfov2 * nearPlane;
	const auto nearHalfWidth = atanfov2 * nearPlane;
	const auto farHalfHeight = tanfov2 * farPlane;
	const auto farHalfWidth = atanfov2 * farPlane;

	//lines->GetBufferObject()->Resize(8 * sizeof(Point3f));
	std::vector<Point3f> ptr( 8 );
	//auto ptr = reinterpret_cast<Point3f*>(lines->GetBufferObject()->LocalData());

	ptr[ 0 ] = nearCenter + nearHalfWidth * ( -right ) + nearHalfHeight * ( -up );
	ptr[ 1 ] = nearCenter + nearHalfWidth * ( right ) + nearHalfHeight * ( -up );
	ptr[ 2 ] = nearCenter + nearHalfWidth * ( right ) + nearHalfHeight * ( up );
	ptr[ 3 ] = nearCenter + nearHalfWidth * ( -right ) + nearHalfHeight * ( up );

	ptr[ 4 ] = farCenter + farHalfWidth * ( -right ) + farHalfHeight * ( -up );
	ptr[ 5 ] = farCenter + farHalfWidth * ( right ) + farHalfHeight * ( -up );
	ptr[ 6 ] = farCenter + farHalfWidth * ( right ) + farHalfHeight * ( up );
	ptr[ 7 ] = farCenter + farHalfWidth * ( -right ) + farHalfHeight * ( up );
	//auto lines = MakeRef<ArrayFloat3>();
	//lines->GetBufferObject()->SetLocalData(ptr, sizeof(ptr));
	return ptr;
}
Ref<Camera> CreateCamera( const std::string &jsonFileName )
{
	using namespace rapidjson;
	std::ifstream ifs( jsonFileName );
	rapidjson::IStreamWrapper isw( ifs );
	Document d;
	d.ParseStream( isw );

	// Camera Params
	// View Matrix: up front center
	// Perspective Matrix: fov, nearPlane, farPlane, aspectRatio

	Point3f pos;
	Vector3f up;
	Point3f center;
	pos.x = GetValueByPointerWithDefault( d, "/camera/viewMatrix/pos/0", 0.0f ).GetFloat();
	pos.y = GetValueByPointerWithDefault( d, "/camera/viewMatrix/pos/1", 0.0f ).GetFloat();
	pos.z = GetValueByPointerWithDefault( d, "/camera/viewMatrix/pos/2", 10.f ).GetFloat();

	up.x = GetValueByPointerWithDefault( d, "/camera/viewMatrix/up/0", 0.f ).GetFloat();
	up.y = GetValueByPointerWithDefault( d, "/camera/viewMatrix/up/1", 1.f ).GetFloat();
	up.z = GetValueByPointerWithDefault( d, "/camera/viewMatrix/up/2", 0.f ).GetFloat();

	center.x = GetValueByPointerWithDefault( d, "/camera/viewMatrix/center/0", 0.f ).GetFloat();
	center.y = GetValueByPointerWithDefault( d, "/camera/viewMatrix/center/1", 0.f ).GetFloat();
	center.z = GetValueByPointerWithDefault( d, "/camera/viewMatrix/center/2", 0.f ).GetFloat();

	const float fov = GetValueByPointerWithDefault( d, "/camera/perspectiveMatrix/fov", 60.f ).GetFloat();
	const float nearPlane = GetValueByPointerWithDefault( d, "/camera/perspectiveMatrix/nearPlane", 0.01f ).GetFloat();
	const float farPlane = GetValueByPointerWithDefault( d, "/camera/perspectiveMatrix/farPlane", 200.f ).GetFloat();
	const float aspectRatio = GetValueByPointerWithDefault( d, "/camera/perspectiveMatrix/aspectRatio", 1024.f / 768.f ).GetFloat();

	auto camera = MakeRef<Camera>( pos, up, center );
	//Transform perspectiveMatrix;
	//perspectiveMatrix.SetGLPerspective(fov, aspectRatio, nearPlane, farPlane);
	//camera->SetProjectionMatrix(perspectiveMatrix);
	camera->SetCamera( pos, up, center, nearPlane, farPlane, aspectRatio, fov );
	return camera;
}

void ConfigCamera( Camera *camera, const std::string &jsonFileName )
{
	if ( !camera ) return;
	//using namespace rapidjson;
	//std::ifstream ifs( jsonFileName );
	//rapidjson::IStreamWrapper isw( ifs );
	//Document d;
	//d.ParseStream( isw );

	std::ifstream json( jsonFileName );
	CameraJSONStruct JSON;
	json >> JSON;

	// Camera Params
	// View Matrix: up front center
	// Perspective Matrix: fov, nearPlane, farPlane, aspectRatio

	Point3f pos;
	Vector3f up;
	Point3f center;

	center.x = JSON.viewMatrix.center[ 0 ];
	center.y = JSON.viewMatrix.center[ 1 ];
	center.z = JSON.viewMatrix.center[ 2 ];

	pos.x = JSON.viewMatrix.pos[ 0 ];
	pos.y = JSON.viewMatrix.pos[ 1 ];
	pos.z = JSON.viewMatrix.pos[ 2 ];

	up.x = JSON.viewMatrix.up[ 0 ];
	up.y = JSON.viewMatrix.up[ 1 ];
	up.z = JSON.viewMatrix.up[ 2 ];

	const float fov = JSON.perspectiveMatrix.fov;
	const float nearPlane = JSON.perspectiveMatrix.nearPlane;
	const float farPlane = JSON.perspectiveMatrix.farPlane;
	const float aspectRatio = JSON.perspectiveMatrix.aspectRatio;

	Transform perspectiveMatrix;
	perspectiveMatrix.SetGLPerspective( fov, aspectRatio, nearPlane, farPlane );

	camera->SetCamera( pos, up, center, nearPlane, farPlane, aspectRatio, fov );

	//camera->SetProjectionMatrix(perspectiveMatrix);
	//camera->SetNearPlane(nearPlane);
	//camera->SetFarPlane(farPlane);
}

bool SaveCameraAsJson( Ref<Camera> camera, const std::string &jsonFileName )
{
	using namespace rapidjson;
	auto pos = camera->GetPosition();
	auto center = camera->GetCenter();
	auto up = camera->GetUp();

	auto fov = camera->GetFov();
	auto nearPlane = camera->GetNearPlane();
	auto farPlane = camera->GetFarPlane();
	auto aspectRatio = camera->GetAspectRatio();

	Document d;

	SetValueByPointer( d, "/viewMatrix/pos/0", pos.x );
	SetValueByPointer( d, "/viewMatrix/pos/1", pos.y );
	SetValueByPointer( d, "/viewMatrix/pos/2", pos.z );

	SetValueByPointer( d, "/viewMatrix/up/0", up.x );
	SetValueByPointer( d, "/viewMatrix/up/1", up.y );
	SetValueByPointer( d, "/viewMatrix/up/2", up.z );

	SetValueByPointer( d, "/viewMatrix/center/0", center.x );
	SetValueByPointer( d, "/viewMatrix/center/1", center.y );
	SetValueByPointer( d, "/viewMatrix/center/2", center.z );

	SetValueByPointer( d, "/perspectiveMatrix/fov", fov );
	SetValueByPointer( d, "/perspectiveMatrix/nearPlane", nearPlane );
	SetValueByPointer( d, "/perspectiveMatrix/farPlane", farPlane );
	SetValueByPointer( d, "/perspectiveMatrix/aspectRatio", aspectRatio );

	std::ofstream ofs( jsonFileName );
	if ( !ofs.is_open() ) {
		::vm::Log( "Can not open file {}.", jsonFileName.c_str() );
		return false;
	}
	OStreamWrapper osw( ofs );
	Writer<OStreamWrapper, UTF8<>, UTF8<>, CrtAllocator, kWriteDefaultFlags> writer( osw );
	return d.Accept( writer );
}

void CameraManipulator::ResizeEvent( int w, int h )
{
	//assert(camera);
	//if (camera)
	//{
	//	auto viewport = camera->GetViewport();
	//	assert(viewport);
	//	if (viewport)
	//	{
	//		viewport->SetViewportSize(w, h);
	//		Transform proj;
	//		proj.SetGLPerspective(60.f, float(w) / float(h), 0.01, 100);
	//		camera->SetProjectionMatrix(proj);
	//	}
	//}
	(void)w;
	(void)h;
}

void CameraManipulator::AddedEvent( RenderContext *context )
{
	SetContext( context );
	// Setup Viewport Size
}

void CameraManipulator::DeletedEvent( RenderContext *context )
{
	SetContext( nullptr );
}

void CameraManipulator::MousePressEvent( MouseButton button, int xpos, int ypos )
{
	lastMousePos.x = xpos;
	lastMousePos.y = ypos;
}

void CameraManipulator::MouseMoveEvent( MouseButton button, int xpos, int ypos )
{
	if ( camera != nullptr ) {
		//Debug("CameraManipulator::MouseMoveEvent");
		// Update Camera
		const float dx = xpos - lastMousePos.x;
		const float dy = lastMousePos.y - ypos;

		if ( dx == 0.0 && dy == 0.0 )
			return;

		if ( m_fpsCamera == false ) {
			if ( ( button & Mouse_Left ) && ( button & Mouse_Right ) ) {
				const auto directionEx = camera->Up() * dy + dx * camera->Right();
				camera->Movement( directionEx, 0.002 );
			} else if ( button == Mouse_Left ) {
				camera->Rotation( dx, dy );
			} else if ( button == Mouse_Right && dy != 0.0 ) {
				const auto directionEx = camera->GetFront() * dy;
				camera->Movement( directionEx, 1.0 );
			}

		} else {
			const auto front = camera->GetFront().Normalized();
			const auto up = camera->GetUp().Normalized();
			const auto right = Vec3f::Cross( front, up );
			const auto dir = ( up * dy - right * dx ).Normalized();
			const auto axis = Vec3f::Cross( dir, front );
			camera->SetFront( Rotate( axis, 5.0 ) * front );
		}

		lastMousePos.x = xpos;
		lastMousePos.y = ypos;
	}
}

void CameraManipulator::KeyPressEvent( KeyButton key )
{
	float sensity = 0.1;
	if ( camera != nullptr ) {
		if ( m_fpsCamera == true ) {
			if ( key == KeyButton::Key_W ) {
				auto dir = camera->GetFront();
				camera->Movement( sensity * dir.Normalized(), 100 );
				//mrtAgt->CreateGetCamera()->Movement();
			} else if ( key == KeyButton::Key_S ) {
				auto dir = -camera->GetFront();
				camera->Movement( sensity * dir.Normalized(), 100 );
			} else if ( key == KeyButton::Key_A ) {
				auto dir = ( Vec3f::Cross( camera->GetUp(), camera->GetFront() ).Normalized() * sensity );
				camera->Movement( dir, 100 );
				camera->SetCenter( camera->GetCenter() + dir );
			} else if ( key == KeyButton::Key_D ) {
				auto dir = ( -Vec3f::Cross( camera->GetUp(), camera->GetFront() ).Normalized() ) * sensity;
				camera->Movement( dir, 100 );
				camera->SetCenter( camera->GetCenter() + dir );
			}
		}
	}
}
}  // namespace vm

}  // namespace ysl

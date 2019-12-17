
#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_

#include "graphictype.h"
#include <VMGraphics/plane.h>
#include <array>
#include <vector>
#include "camera.h"

namespace vm
{
class VISUALMAN_EXPORT_IMPORT Frustum
{
	std::array<Plane, 6> planes;

public:
	Frustum( const vm::Camera &camera );
	const Plane &GetFrontPlane() const { return planes[ 0 ]; }
	const Plane &GetBackPlane() const { return planes[ 1 ]; }
	const Plane &GetLeftPlane() const { return planes[ 2 ]; }
	const Plane &GetRightPlane() const { return planes[ 3 ]; }
	const Plane &GetTopPlane() const { return planes[ 4 ]; }
	const Plane &GetBottomPlane() const { return planes[ 5 ]; }
	std::vector<Point2f> GetFrustumLines() const;

private:
	void Init( const vm::Camera &camera );
};
}  // namespace ysl
#endif

#ifndef PATH_TRACER_H
#define PATH_TRACER_H
#endif

#include "../graphicfoundation/spectrum.h"

namespace vm
{
class Scene;
class Ray;

RGBASpectrum trace( const Scene &scene,
					const Ray &ray,
					int depth, bool di, bool gi, int intensity );

}  // namespace ysl
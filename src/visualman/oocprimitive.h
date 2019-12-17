
#ifndef _LARGEVOLUME_H_
#define _LARGEVOLUME_H_
#include "primitive.h"
#include <memory>

namespace vm
{
class OutOfCorePrimitive;

struct VISUALMAN_EXPORT_IMPORT IOutOfCoreAdapter : std::enable_shared_from_this<IOutOfCoreAdapter>
{
	virtual void OnDrawCallStart( OutOfCorePrimitive *p ) = 0;
	virtual void OnDrawCallFinished( OutOfCorePrimitive *p ) = 0;
	virtual ~IOutOfCoreAdapter() = default;
};

class VISUALMAN_EXPORT_IMPORT OutOfCorePrimitive : public Primitive
{
public:
	OutOfCorePrimitive();

	void Render( const Actor *actor, const Shading *shading, const Camera *camera, RenderContext *context ) override;
	void SetOutOfCoreResources( VMRef<IOutOfCoreAdapter> e );
	void RemoveOutOfCoreResources( VMRef<IOutOfCoreAdapter> e );
	void RemoveOutOfCoreResources();
	void SetRenderFinished( bool finished ) { renderFinished = finished; }

private:
	VMRef<IOutOfCoreAdapter> callbacks;
	bool renderFinished = false;
};
}  // namespace vm
#endif
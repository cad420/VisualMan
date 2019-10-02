
#ifndef _ABSTRADRAW_H_
#define _ABSTRADRAW_H_

#include "graphictype.h"
#include <vector>

namespace ysl
{
namespace vm
{
class VISUALMAN_EXPORT_IMPORT AbstrDrawCall
{
public:
	AbstrDrawCall() :
	  drawType( PT_TRIANGLES ) {}
	virtual void Render() const = 0;
	virtual void UpdateDirtyBufferObject( BufferObjectUpdateMode mode ) = 0;
	virtual void DestroyBufferObject() = 0;
	void SetLineWidth( float width ) { lineWidth = width; }
	float GetLineWidth() const { return lineWidth; }
	void SetPrimitiveType( PrimitiveType type ) { drawType = type; }
	PrimitiveType GetPrimitiveType() const { return drawType; }
	virtual ~AbstrDrawCall() = default;

private:
	PrimitiveType drawType;
	float lineWidth = 1.0f;
	//std::vector<Ref<IDrawCallEvent>> events;
};
}  // namespace vm
}  // namespace ysl
#endif
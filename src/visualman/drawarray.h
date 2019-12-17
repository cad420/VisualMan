
#ifndef _DRAWARRAY_H_
#define _DRAWARRAY_H_
#include <iostream>
#include "abstrdraw.h"

namespace vm
{
class DrawArray : public AbstrDrawCall
{
public:
	DrawArray( int start, int count );

	void Render() const override;
	void UpdateDirtyBufferObject( BufferObjectUpdateMode mode ) override {}
	void DestroyBufferObject() override {}
	int Start() const { return start; }
	int Count() const { return count; }

private:
	int start = 0;
	int count = 0;
};
}  // namespace vm
#endif
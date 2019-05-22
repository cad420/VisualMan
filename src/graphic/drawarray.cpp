
#include "drawarray.h"
#include "../../lib/gl3w/GL/gl3w.h"
#include "../opengl/openglutils.h"

namespace ysl {
	namespace vm
	{
		DrawArray::DrawArray(int start, int count): start(start), count(count)
		{

		}

		void DrawArray::Render() const
		{
			GL(glDrawArrays(GetPrimitiveType(), start, count));
		}
	}
}
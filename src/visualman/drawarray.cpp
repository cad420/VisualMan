
//#include <GL/gl3w.h>
#include "graphictype.h"
#include "drawarray.h"

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
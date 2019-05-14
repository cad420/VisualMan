
#ifndef _DRAW_H_
#define _DRAW_H_

#include "graphictype.h"

namespace ysl
{
	namespace gpu
	{

		enum EDrawType
		{
			DT_TRIANGLES
		};

		class GRAPHICS_EXPORT_IMPORT AbstrDrawCall
		{
		public:
			AbstrDrawCall():drawType(DT_TRIANGLES){}
			virtual void Render()const = 0;
			virtual ~AbstrDrawCall() = default;
		private:
			EDrawType drawType;
		};
	}
}
#endif
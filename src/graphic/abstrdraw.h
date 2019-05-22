
#ifndef _ABSTRADRAW_H_
#define _ABSTRADRAW_H_

#include "graphictype.h"

namespace ysl
{
	namespace vm
	{

		class GRAPHICS_EXPORT_IMPORT AbstrDrawCall
		{
		public:
			AbstrDrawCall():drawType(PT_TRIANGLES){}

			virtual void Render()const = 0;

			void SetPrimitiveType(PrimitiveType type) { drawType = type; }
			PrimitiveType GetPrimitiveType()const { return drawType; }
			virtual ~AbstrDrawCall() = default;
		private:
			PrimitiveType drawType;
		};
	}
}
#endif
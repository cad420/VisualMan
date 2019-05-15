
#ifndef _DRAWARRAY_H_
#define _DRAWARRAY_H_
#include <iostream>
#include "abstrdraw.h"

namespace ysl
{
	namespace graphics
	{
		class GRAPHICS_EXPORT_IMPORT DrawArray:public AbstrDrawCall
		{
		public:
			DrawArray() = default;
			void Render()const override
			{
				std::cout << "Draw Array Executed\n";
				// glDrawArray;

			}
			int Start()const { return start; }
			int Count()const { return count; }
		private:
			int start = 0;
			int count = 0;
		};
	}
}
#endif

#ifndef _DRAWARRAY_H_
#define _DRAWARRAY_H_
#include <iostream>
#include "abstrdraw.h"

namespace ysl
{
	namespace gpu
	{
		class GRAPHICS_EXPORT_IMPORT DrawArray:public AbstrDrawCall
		{
		public:
			DrawArray();
			void Render()const override
			{
				std::cout << "Draw Array Executed\n";
			}
			int Start()const { return start; }
			int Count()const { return count; }
		private:
			int start;
			int count;

		};
	}
}
#endif
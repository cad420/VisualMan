
#ifndef _LIBRARY_H_
#define _LIBRARY_H_
#include <string>
#include "common.h"

namespace ysl
{
		class COMMON_EXPORT_IMPORT Library
		{
		public:
			Library(const std::string & name);
			void * Symbol(const std::string & name)const;
			void Close();
			~Library();
		private:
			void * lib;
		};
}

#endif
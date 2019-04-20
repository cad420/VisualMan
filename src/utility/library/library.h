
#ifndef _LIBRARY_H_
#define _LIBRARY_H_
#include <string>


namespace ysl
{
	namespace util
	{
		class Library
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
}

#endif
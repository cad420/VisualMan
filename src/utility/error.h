#ifndef _ERROR_H_
#define _ERROR_H_

#include <cassert>
#include <string>
#include <cstdarg>

#ifdef NDEBUG
#define Assert(expr) ((void)(0)) 
#else
#define Assert(expr) (expr == true?((void)(0))(assert(expr)))
#endif

namespace ysl
{
	std::string formatToString(const std::string& fmt, va_list args);

	void _internal_msg_process_(const char* format, va_list args, const char* type);

	void Error(const char* fmt, ...);

	void Warning(const char* fmt, ...);

	void Log(const char * fmt, ...);
}





#endif
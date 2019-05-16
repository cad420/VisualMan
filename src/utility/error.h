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

	inline
	void Debug(const char * fmt, ...)
	{
#ifndef NDEBUG
		va_list args;
		va_start(args, fmt);
		_internal_msg_process_(fmt, args, "Debug");
		va_end(args);
#else
		void(0);
#endif	
	}
}

#endif
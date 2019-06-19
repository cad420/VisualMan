#include "error.h"

namespace ysl
{
	std::string formatToString(const std::string& fmt, va_list args)
	{
		va_list argsCopy;
		va_copy(argsCopy, args);
		size_t size = vsnprintf(nullptr, 0, fmt.c_str(), args) + 1;

		std::string str;
		str.resize(size);

		vsnprintf(&str[0], size, fmt.c_str(), argsCopy);

		str.pop_back();
		return str;
	}

	void _internal_msg_process_(const char* format, va_list args, const char* type,bool sameLine)
	{
		std::string msg;
		msg += type;
		msg = formatToString(format, args);
		if(sameLine)
			fprintf(stderr, "%s\r", msg.c_str());
		else
			fprintf(stderr, "%s\n", msg.c_str());

	}


	void Error(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		_internal_msg_process_(fmt, args, "Error",false);
		va_end(args);
		abort();
	}

	void Warning(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		_internal_msg_process_(fmt, args, "Warning",false);
		va_end(args);
	}

	void Log(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		_internal_msg_process_(fmt, args, "Log",false);
		va_end(args);
	}

	void Display(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		_internal_msg_process_(fmt, args, "Display",true);
		va_end(args);
	}
}

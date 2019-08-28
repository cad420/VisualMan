
#include "library.h"
#include <cassert>
#include <iostream>
#include "error.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/times.h>
#include <dlfcn.h>
#endif

namespace ysl
{
	Library::Library(const std::string& name)
	{
		std::string errorMsg;
#ifdef _WIN32
		lib = LoadLibrary(name.c_str());
		if (!lib)
		{
			DWORD err = GetLastError();
			LPTSTR lpMsgBuf;
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				err,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)& lpMsgBuf,
				0, NULL);

			errorMsg = lpMsgBuf;
			LocalFree(lpMsgBuf);
		}


#elif defined(__MACOSX__) || defined(__APPLE__)
		lib = dlopen(name.c_str(), RTLD_NOW | RTLD_GLOBAL);
		if (!lib)
			errorMsg = dlerror();
#elif defined(__linux__)
		lib = dlopen(name.c_str(), RTLD_NOW | RTLD_GLOBAL);
		if (!lib)
			errorMsg = dlerror();
#endif	/*defined(__MACOSX__) || defined(__APPLE__)*/
		if (!lib)
		{
			Debug("%s can bot be found.", name.c_str());
			throw std::runtime_error(errorMsg);
		}
	}
	
	void* Library::Symbol(const std::string& name) const
	{
		assert(lib);
#ifdef _WIN32
		return GetProcAddress((HMODULE)lib, name.c_str());
#else
		return dlsym(lib, name.c_str());
#endif
	}

	void Library::Close()
	{
#ifdef _WIN32
		FreeLibrary((HMODULE)lib);
#else
		dlclose(lib);
#endif
	}
	Library::~Library()
	{
		Close();
	}
}
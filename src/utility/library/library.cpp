
#include "library.h"
#include <cassert>

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/times.h>
#include <dlfcn.h>
#endif

namespace ysl
{
	namespace util
	{
		Library::Library(const std::string& name)
		{
			std::string errorMsg;
			std::string fullName;
#ifdef _WIN32
			fullName = name + ".dll";
			lib = LoadLibrary(fullName.c_str());
			if(!lib)
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
					(LPTSTR)&lpMsgBuf,
					0, NULL);

				errorMsg = lpMsgBuf;
				LocalFree(lpMsgBuf);
			}


#else
#if defined(__MACOSX__) || defined(__APPLE__)
			fullName = "lib" + name + ".dylib";		// mac extension
#else defined(__linux__)
			fullName = "lib" + name + ".so";			// linux extension
#endif	/*defined(__MACOSX__) || defined(__APPLE__)*/
			lib = dlopen(fullName.c_str(), RTLD_NOW | RTLD_GLOBAL);
			if(!lib)
				errorMsg = dlerror();
#endif /*_WIN32*/

			if(!lib)
			{
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
}
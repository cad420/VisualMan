#ifndef _COMMON_H_
#define _COMMON_H_

#if defined(_WIN32) && defined(VM_SHARED_LIBRARY)
	#ifdef common_EXPORTS
		#define COMMON_EXPORT_IMPORT __declspec(dllexport)
	#else
		#define COMMON_EXPORT_IMPORT __declspec(dllimport)
	#endif
#else
	#define COMMON_EXPORT_IMPORT
#endif


namespace ysl
{
	
}
#endif

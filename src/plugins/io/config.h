#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef _WIN32
	#ifdef ioplugin_EXPORTS
		#define IO_EXPORT_IMPORT __declspec(dllexport) 
	#else
		#define IO_EXPORT_IMPORT __declspec(dllimport)
	#endif
#else
	#define IO_EXPORT_IMPORT
#endif

#endif

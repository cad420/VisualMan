#ifndef _CONFIG_H_
#define _CONFIG_H_

#if defined(_WIN32) && defined(VM_SHARED_LIBRARY)

#ifdef vmapps_EXPORTS
#define VMAPPS_EXPORT_IMPORT __declspec(dllexport)
#else
#define VMAPPS_EXPORT_IMPORT __declspec(dllimport)
#endif

#else
#define VMAPPS_EXPORT_IMPORT 
#endif

#endif
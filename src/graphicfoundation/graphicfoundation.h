
#ifndef _GRAPHICFOUNDATION_H_
#define _GRAPHICFOUNDATION_H_

#if defined(_WIN32) && defined(VM_SHARED_LIBRARY)
#ifdef graphicfoundation_EXPORTS
#define GRAPHIC_FOUNDATION_EXPORT_IMPORT __declspec(dllexport)
#else
#define GRAPHIC_FOUNDATION_EXPORT_IMPORT __declspec(dllimport)
#endif
#else
#define GRAPHIC_FOUNDATION_EXPORT_IMPORT
#endif

namespace ysl
{

}

#endif

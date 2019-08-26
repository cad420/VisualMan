#ifndef _FILEMAPPINGPLUGININTERFACE_H_
#define _FILEMAPPINGPLUGININTERFACE_H_
#include <string>
#include "object.h"
#include "common.h"

namespace ysl
{
	enum class FileAccess
	{
		Read,// = GENERIC_READ,
		Write,//= GENERIC_WRITE,
		ReadWrite
	};
	enum class MapAccess
	{
		ReadOnly,// = PAGE_READONLY,
		ReadWrite //= PAGE_READWRITE
	};

	class COMMON_EXPORT_IMPORT IFileMappingPluginInterface:public Object
	{
		DECLARE_RTTI
	public:
		IFileMappingPluginInterface() = default;
		virtual bool Open(const std::string & fileName, size_t fileSize, FileAccess fileFlags, MapAccess mapFlags) = 0;
		virtual unsigned char* FileMemPointer(unsigned long long offset, std::size_t size) = 0;
		virtual void DestroyFileMemPointer(unsigned char* addr) = 0;
		virtual bool WriteCommit() = 0;
		virtual bool Close() = 0;
		virtual ~IFileMappingPluginInterface() = default;
	};
}
#endif
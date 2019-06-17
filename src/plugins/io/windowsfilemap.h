
#ifndef _WINDOWSFILEMAP_H_
#define _WINDOWSFILEMAP_H_
#include <rawio.h>
#include <unordered_set>
#include "config.h"

#ifdef _WIN32
#include <Windows.h>

namespace ysl
{
	class IO_EXPORT_IMPORT WindowsFileMapping:public IPluginFileMap
	{

		DECLARE_RTTI
		DECLARE_INITIAL(WindowsFileMapping)
		HANDLE f = nullptr;
		HANDLE mapping = nullptr;
		FileAccess fileFlag;
		MapAccess mapFlag;
		void * addr = nullptr;
		std::unordered_set<unsigned char*> mappedPointers;
		void PrintLastErrorMsg();
	public:
		WindowsFileMapping() = default;
		bool Open(const std::string& fileName, size_t fileSize, FileAccess fileFlags, MapAccess mapFlags) override;
		unsigned char* FileMemPointer(unsigned long long offset, std::size_t size) override;
		void DestroyFileMemPointer(unsigned char* addr) override;
		bool WriteCommit() override;
		bool Close() override;
		~WindowsFileMapping();
	};

}

#endif /*_WIN32*/

#endif /*_WINDOWSFILEMAP_H_*/

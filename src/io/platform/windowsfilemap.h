
#ifndef _WINDOWSFILEMAP_H_
#define _WINDOWSFILEMAP_H_
#include "../rawio.h"

#ifdef _WIN32

#include <Windows.h>

namespace ysl
{
	class WindowsFileMapping :
	public AbstraFileMap
	{
		HANDLE f;
		HANDLE mapping;
		void * addr;
		std::unordered_set<unsigned char*> mappedPointers;
		int fileAccess;
		int mapAccess;
		void PrintLastErrorMsg();
	public:
		enum FileAccess
		{
			Read = GENERIC_READ,
			Write = GENERIC_WRITE,
		};
		enum MapAccess
		{
			ReadOnly = PAGE_READONLY,
			ReadWrite = PAGE_READWRITE
		};
	public:
		WindowsFileMapping(const std::string & fileName, std::size_t fileSize, int FileAccessFlags, int MapAccessFlags);
		unsigned char* FileMemPointer(unsigned long long offset, std::size_t size) override;
		void DestroyFileMemPointer(unsigned char* addr) override;
		bool WriteCommit() override;
		bool Close() override;
		~WindowsFileMapping();

	};
}


#endif /*_WIN32*/


#endif /*_WINDOWSFILEMAP_H_*/

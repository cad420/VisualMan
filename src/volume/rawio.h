#ifndef _RAWIO_H_
#define _RAWIO_H_
#include "../mathematics/geometry.h"
#include <unordered_map>
#include <unordered_set>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace ysl
{

	class AbstrRawIO
	{
	protected:
		std::string fileName;
		std::size_t fileSize;
		int flags;
	public:
		AbstrRawIO(const std::string & fileName, std::size_t fileSize,int flags);
		virtual unsigned char* FileMemPointer(unsigned long long offset, std::size_t size) = 0;
		virtual void DestroyFileMemPointer(unsigned char* addr) = 0;
		virtual bool WriteCommit() = 0;
		virtual bool Close()=0;
		virtual ~AbstrRawIO() {}
	};

#ifdef _WIN32

	class WindowsMappingRawIO :public AbstrRawIO
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
		WindowsMappingRawIO(const std::string & fileName, std::size_t fileSize,int FileAccessFlags, int MapAccessFlags);

		unsigned char* FileMemPointer(unsigned long long offset, std::size_t size) override;

		void DestroyFileMemPointer(unsigned char* addr) override;

		bool WriteCommit() override;

		bool Close() override;

		~WindowsMappingRawIO();
	};
#endif

}
#endif
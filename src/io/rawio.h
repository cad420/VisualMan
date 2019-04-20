#ifndef _RAWIO_H_
#define _RAWIO_H_
#include "../mathematics/geometry.h"
#include <unordered_map>
#include <unordered_set>
#include "reflection.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef __linux__
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#endif

namespace ysl
{

	class AbstraFileMap:public Reflectable
	{
	protected:
		std::string fileName;
		std::size_t fileSize;
		int flags;
	public:
		AbstraFileMap(const std::string & fileName, std::size_t fileSize,int flags);
		virtual unsigned char* FileMemPointer(unsigned long long offset, std::size_t size) = 0;
		virtual void DestroyFileMemPointer(unsigned char* addr) = 0;
		virtual bool WriteCommit() = 0;
		virtual bool Close()=0;
		virtual ~AbstraFileMap() {}
	};

#ifdef _WIN32

	class WindowsFileMapping :public AbstraFileMap
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
		WindowsFileMapping(const std::string & fileName, std::size_t fileSize,int FileAccessFlags, int MapAccessFlags);
		unsigned char* FileMemPointer(unsigned long long offset, std::size_t size) override;
		void DestroyFileMemPointer(unsigned char* addr) override;
		bool WriteCommit() override;
		bool Close() override;
		~WindowsFileMapping();
	};
#endif

#ifdef __linux__

class LinuxMappingRawIO:public AbstraFileMap{
	public:
	LinuxMappingRawIO(const std::string & fileName,std::size_t fileSize);
	unsigned char * FileMemPointer(unsigned long long offset,std::size_t size)override;
	void DestroyFileMemPointer(unsigned char * addr)override;
	bool WriteCommit()override;
	bool Close()override;
	~LinuxMappingRawIO();
};

#endif

}
#endif
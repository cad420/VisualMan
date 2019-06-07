#ifndef _RAWIO_H_
#define _RAWIO_H_
#include <string>

//#ifdef _WIN32
//#include <Windows.h>
//#endif

namespace ysl
{

	class AbstraFileMap//:public Reflectable
	{
	protected:
		std::string fileName;
		std::size_t fileSize;
		int flags;
	public:
		AbstraFileMap(const std::string & fileName, std::size_t fileSize,int flags):fileName(fileName), fileSize(fileSize), flags(flags){}
		virtual unsigned char* FileMemPointer(unsigned long long offset, std::size_t size) = 0;
		virtual void DestroyFileMemPointer(unsigned char* addr) = 0;
		virtual bool WriteCommit() = 0;
		virtual bool Close()=0;
		virtual ~AbstraFileMap() = default;
	};

}
#endif
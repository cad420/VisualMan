#ifndef _LINUXFILEMAP_H_
#define _LINUXFILEMAP_H_

#include "../rawio.h"

#ifdef __linux__

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

namespace ysl
{
	class LinuxMappingRawIO :public AbstraFileMap
	{
	public:
		LinuxMappingRawIO(const std::string & fileName, std::size_t fileSize);
		unsigned char * FileMemPointer(unsigned long long offset, std::size_t size)override;
		void DestroyFileMemPointer(unsigned char * addr)override;
		bool WriteCommit()override;
		bool Close()override;
		~LinuxMappingRawIO();
	};
}


#endif/*__linux__*/

#endif/*_LINUXFILEMAP_H_*/
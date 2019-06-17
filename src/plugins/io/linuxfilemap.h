#ifndef _LINUXFILEMAP_H_
#define _LINUXFILEMAP_H_


#ifdef __linux__
#include "config.h"
#include <rawio.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <set>

namespace ysl
{

    class IO_EXPORT_IMPORT LinuxFileMapping:public IPluginFileMap
    {
        DECLARE_RTTI
        DECLARE_INITIAL(LinuxFileMapping)

        std::set<std::pair<void*,size_t>> ptrs;
        int fd = -1;
        FileAccess fileAccess;
        MapAccess mapAccess;
        public:
        bool Open(const std::string & fileName,size_t fileSize,FileAccess fileFlags,MapAccess mapFlags)override;
        unsigned char * FileMemPointer(unsigned long long offset, size_t size)override;
        void DestroyFileMemPointer(unsigned char * addr)override;
        bool WriteCommit()override;
        bool Close()override;
        ~LinuxFileMapping();
    };
}


#endif/*__linux__*/

#endif/*_LINUXFILEMAP_H_*/
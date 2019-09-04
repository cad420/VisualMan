#ifndef _LINUXFILEMAP_H_
#define _LINUXFILEMAP_H_

#ifdef __linux__
#include "config.h"
#include <filemappingplugininterface.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <set>
#include <plugin.h>

namespace ysl
{
    class IO_EXPORT_IMPORT LinuxFileMapping:public IFileMappingPluginInterface
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
	

	class LinuxFileMappingFactory :public IPluginFactory
	{
		DECLARE_PLUGIN_FACTORY("common.io")
	public:
		std::vector<std::string> Keys() const override;
		std::unique_ptr<Object> Create(const std::string& key) override;
	};
	
}


#endif/*__linux__*/

#endif/*_LINUXFILEMAP_H_*/
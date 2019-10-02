#ifndef _LINUXFILEMAP_H_
#define _LINUXFILEMAP_H_

#ifdef __linux__
#include <set>
#include <memory>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <VMCoreExtension/ifilemappingplugininterface.h>
#include <VMCoreExtension/plugin.h>
#include <VMCoreExtension/plugindef.h>
#include "config.h"

namespace ysl
{
class LinuxFileMapping : public ::vm::EverythingBase<IFileMapping>
{
	std::set<std::pair<void *, size_t>> ptrs;
	int fd = -1;
	FileAccess fileAccess;
	MapAccess mapAccess;

public:
	LinuxFileMapping( ::vm::IRefCnt *cnt ) :
	  ::vm::EverythingBase<IFileMapping>( cnt ) {}
	bool Open( const std::string &fileName, size_t fileSize, FileAccess fileFlags, MapAccess mapFlags ) override;
	unsigned char *FileMemPointer( unsigned long long offset, size_t size ) override;
	void DestroyFileMemPointer( unsigned char *addr ) override;
	bool WriteCommit() override;
	bool Close() override;
	~LinuxFileMapping();
};

class LinuxFileMappingFactory : public IPluginFactory
{
	DECLARE_PLUGIN_FACTORY( "visualman.io" )
public:
	std::vector<std::string> Keys() const override;
	::vm::IEverything *Create( const std::string &key ) override;
};

}  // namespace ysl

#endif /*__linux__*/

#endif /*_LINUXFILEMAP_H_*/

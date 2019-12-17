#include <VMUtils/vmnew.hpp>
#ifdef __linux__
#include "linuxfilemap.h"

namespace vm
{
bool LinuxFileMapping::Open( const std::string &fileName, size_t fileSize, FileAccess fileFlags, MapAccess mapFlags )
{
	Close();

	fileAccess = fileFlags;
	mapAccess = mapFlags;

	int fileflags = 0;
	if ( fileFlags == FileAccess::Read )
		fileflags |= O_RDONLY;
	if ( fileFlags == FileAccess::Write )
		fileflags |= O_WRONLY;
	if ( fileFlags == FileAccess::ReadWrite )
		fileflags |= O_RDWR;

	fileflags |= O_CREAT;  // If file don't exist, create a new one.
	fd = open64( fileName.c_str(), fileflags, 0777 );

	if ( -1 == fd ) {
		//throw std::runtime_error("can not open file");
		return false;
	}

	return true;
}
unsigned char *LinuxFileMapping::FileMemPointer( unsigned long long offset, size_t size )
{
	int prot = 0;
	if ( mapAccess == MapAccess::ReadOnly )
		prot = PROT_READ;
	if ( mapAccess == MapAccess::ReadWrite )
		prot = PROT_READ | PROT_WRITE;
	void *ptr = mmap( nullptr, size, prot, MAP_SHARED, fd, offset );
	return reinterpret_cast<unsigned char *>( ptr );
}
void LinuxFileMapping::DestroyFileMemPointer( unsigned char *addr )
{
	for ( auto it = ptrs.begin(); it != ptrs.end(); ) {
		if ( it->first == addr ) {
			munmap( it->first, it->second );
			it = ptrs.erase( it );
		} else {
			++it;
		}
	}
}
bool LinuxFileMapping::WriteCommit()
{
	return true;
}
bool LinuxFileMapping::Close()
{
	for ( auto it = ptrs.begin(); it != ptrs.end(); ) {
		munmap( it->first, it->second );
		it = ptrs.erase( it );
	}
	if ( fd != -1 ) {
		close( fd );
		fd = -1;
	}
	return true;
}
LinuxFileMapping::~LinuxFileMapping()
{
	Close();
}

std::vector<std::string> LinuxFileMappingFactory::Keys() const
{
	return { "linux" };
}

::vm::IEverything *LinuxFileMappingFactory::Create( const std::string &key )
{
	return VM_NEW<LinuxFileMapping>();
}
}  // namespace ysl

#endif /*__linux__*/

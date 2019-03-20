
#include "rawio.h"
#include <string>
#include "../mathematics/geometry.h"

#ifdef _WIN32
#include <Windows.h>
#endif


namespace ysl
{
	AbstrRawIO::AbstrRawIO(const std::string& fileName, const ysl::Size3& dimensions, size_t voxelSize,int flags) :fileName(fileName), dimensions(dimensions), voxelSize(voxelSize),flags(flags)
	{

	}

	

#ifdef _WIN32
	WindowsMappingRawIO::WindowsMappingRawIO(const std::string& fileName, const ysl::Size3& dimensions,
		size_t voxelSize,int FileAccessFlags,int MapAccessFlags) :AbstrRawIO(fileName, dimensions, voxelSize,flags),addr(nullptr)
	{

		f = CreateFile(TEXT(fileName.c_str()),
			FileAccessFlags,
			0,
			NULL, 
			OPEN_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (f == INVALID_HANDLE_VALUE)
		{
			printf("Create File failed: %d\n", GetLastError());
			return;
		}
		
		LARGE_INTEGER fileSize;
		fileSize.QuadPart = dimensions.x*dimensions.y*dimensions.z * voxelSize;

		mapping = CreateFileMapping(f,
			NULL, 
			MapAccessFlags, 
			fileSize.HighPart, 
			fileSize.LowPart, 
			NULL);

		if (mapping == nullptr)
		{
			printf("Create File Mapping failed: %d\n", GetLastError());
			return;
		}
	}

	void* WindowsMappingRawIO::FileMemPointer(unsigned long long offset, std::size_t size)
	{
		
		LARGE_INTEGER os;
		os.QuadPart = offset;
		addr = MapViewOfFile(mapping, 
			FILE_MAP_READ, 
			os.HighPart,
			os.LowPart, 
			static_cast<SIZE_T>(size));

		if (!addr)
		{
			printf("MapViewOfFile failed: %d\n", GetLastError());
			return nullptr;
		}
		return addr;
	}

	void WindowsMappingRawIO::DestroyFileMemPointer(void * addr)
	{
		UnmapViewOfFile((LPVOID)addr);
	}

	bool WindowsMappingRawIO::WriteCommit()
	{
		return true;
	}

	WindowsMappingRawIO::~WindowsMappingRawIO()
	{
		WindowsMappingRawIO::DestroyFileMemPointer(addr);
		CloseHandle(f);
		CloseHandle(mapping);
	}
#endif /*_WIN32*/
}

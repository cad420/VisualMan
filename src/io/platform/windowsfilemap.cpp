#ifdef _WIN32

#include "windowsfilemap.h"


namespace ysl
{
	void WindowsFileMapping::PrintLastErrorMsg()
	{
		DWORD dw = GetLastError();
		char msg[512];
		//LPWSTR;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			msg,
			0, NULL);
		printf("[%d]%s\n", dw, msg);
	}

	WindowsFileMapping::WindowsFileMapping(const std::string & fileName,
		std::size_t fileSize,
		int FileAccessFlags,
		int MapAccessFlags) :
		AbstraFileMap(fileName, fileSize, flags),
		addr(nullptr),
		fileAccess(FileAccessFlags),
		mapAccess(MapAccessFlags)
	{
		// Checking if the file is new created first
		bool newCreated = false;
		DWORD dwAttrib = GetFileAttributes(fileName.c_str());

		if (!(dwAttrib != INVALID_FILE_ATTRIBUTES && 0 == (FILE_ATTRIBUTE_DIRECTORY & dwAttrib)))
		{
			newCreated = true;
		}

		f = CreateFile(TEXT(fileName.c_str()),
			FileAccessFlags,
			0,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (f == INVALID_HANDLE_VALUE)
		{
			printf("Create file failed:");
			PrintLastErrorMsg();
			return;
		}

		/// NOTE: LARGE_INTEGER is a signed 64bit integer, but fileSize is an unsigned 64-bit integer
		LARGE_INTEGER fs;
		fs.QuadPart = fileSize;

		if (newCreated) // Specified the file size for file mapping
		{
			SetFilePointer(f, fs.LowPart, &fs.HighPart, FILE_BEGIN);
			if (!SetEndOfFile(f))
			{
				printf("Set end of file failed:");
				PrintLastErrorMsg();
				return;
			}
		}


		mapping = CreateFileMapping(f,
			NULL,
			MapAccessFlags,
			fs.HighPart,
			fs.LowPart,
			NULL);

		if (mapping == nullptr)
		{
			printf("Create file mapping failed");
			PrintLastErrorMsg();
			return;
		}

	}

	unsigned char* WindowsFileMapping::FileMemPointer(unsigned long long offset, std::size_t size)
	{

		LARGE_INTEGER os;
		os.QuadPart = offset;

		int flags = 0;
		if (mapAccess == MapAccess::ReadOnly)
			flags = FILE_MAP_READ;
		if (mapAccess == MapAccess::ReadWrite)
			flags = FILE_MAP_READ | FILE_MAP_WRITE;

		auto addr = (unsigned char*)MapViewOfFile(mapping,
			flags,
			os.HighPart,
			os.LowPart,
			static_cast<SIZE_T>(size));

		if (!addr)
		{
			printf("MapViewOfFile failed:");
			PrintLastErrorMsg();
			return nullptr;
		}

		mappedPointers.insert(addr);
		return addr;
	}

	void WindowsFileMapping::DestroyFileMemPointer(unsigned char* addr)
	{
		auto it = mappedPointers.find(addr);
		if(it != mappedPointers.end())
		{
			UnmapViewOfFile((LPVOID)addr);
			mappedPointers.erase(it);
		}
	}

	bool WindowsFileMapping::WriteCommit()
	{
		return true;
	}

	bool WindowsFileMapping::Close()
	{
		//for (auto & addr : mappedPointers)
		//	WindowsFileMapping::DestroyFileMemPointer(addr);
		CloseHandle(f);
		CloseHandle(mapping);
		return true;
	}

	WindowsFileMapping::~WindowsFileMapping()
	{
		WindowsFileMapping::Close();
	}



}

#endif /*_WIN32*/

#ifndef _WINDOWSFILEMAP_H_
#define _WINDOWSFILEMAP_H_
#include <unordered_set>
#include <VMFoundation/plugin.h>
#include <VMCoreIO/filemappingplugininterface.h>

#ifdef _WIN32
#include <Windows.h>

namespace ysl
{
	class WindowsFileMapping:public ::vm::EverythingBase<IFileMapping>
	{
		HANDLE f = nullptr;
		HANDLE mapping = nullptr;
		FileAccess fileFlag;
		MapAccess mapFlag;
		void * addr = nullptr;
		std::unordered_set<unsigned char*> mappedPointers;
		void PrintLastErrorMsg();
	public:
		WindowsFileMapping(::vm::IRefCnt * cnt): ::vm::EverythingBase<ysl::IFileMapping>(cnt){}
		bool Open(const std::string& fileName, size_t fileSize, FileAccess fileFlags, MapAccess mapFlags) override;
		unsigned char* FileMemPointer(unsigned long long offset, std::size_t size) override;
		void DestroyFileMemPointer(unsigned char* addr) override;
		bool WriteCommit() override;
		bool Close() override;
		~WindowsFileMapping();
	};

	class WindowsFileMappingFactory:public IPluginFactory
	{
		DECLARE_PLUGIN_FACTORY("visualman.io")
	public:
		std::vector<std::string> Keys() const override;
		std::unique_ptr<Object> Create(const std::string& key) override;
		::vm::IEverything* CreateEx(const std::string& key) override;
	};

}

EXPORT_PLUGIN_FACTORY(WindowsFileMappingFactory)

#endif /*_WIN32*/

#endif /*_WINDOWSFILEMAP_H_*/

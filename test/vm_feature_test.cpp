
#include <iostream>
#include <libraryloader.h>
#include <pluginloader.h>
#include <pagefileinterface.h>
int main()
{
	ysl::PluginLoader::LoadPlugins("plugins");
	auto lz4Reader = ysl::PluginLoader::GetPluginLoader()->CreatePlugin<ysl::I3DBlockFilePluginInterface>(".lz4");
	lz4Reader->Open(R"(D:\test.lz4)");
	auto lvdReader = ysl::PluginLoader::GetPluginLoader()->CreatePlugin<ysl::I3DBlockFilePluginInterface>(".lvd");
	lvdReader->Open(R"(D:\mouselod5.lvd)");


	std::cout << lz4Reader->Get3DPageCount() << " " << lvdReader->Get3DPageCount() << std::endl;
	std::cout << lz4Reader->Get3DPageSize() << " " << lvdReader->Get3DPageSize() << std::endl;
	std::cout << lz4Reader->Get3DPageSizeInLog() << " " << lvdReader->Get3DPageSizeInLog() << std::endl;
	std::cout << lz4Reader->GetDataSizeWithoutPadding() << " " << lvdReader->GetDataSizeWithoutPadding() << std::endl;
	std::cout << lz4Reader->GetPadding() << " " << lvdReader->GetPadding() << std::endl;
	std::cout << lz4Reader->GetPhysicalPageCount() << " " << lvdReader->GetPhysicalPageCount() << std::endl;
	std::cout << lz4Reader->GetPageSize() << " " << lvdReader->GetPageSize() << std::endl;
	std::cout << lz4Reader->GetVirtualPageCount() << " " << lvdReader->GetVirtualPageCount() << std::endl;

	const auto blockSie = lvdReader->Get3DPageSize().Prod();
	std::cout << "blockSize:" << blockSie << std::endl;

	for (int i = 0; i < lvdReader->GetPhysicalPageCount(); i++)
	{
		const char* buf1 = (const char*)lvdReader->GetPage(i);
		const char* buf2 = (const char*)lz4Reader->GetPage(i);
		for (int j = 0; j < blockSie; j++)
		{
			if(buf1[j] != buf2[j])
			{
				std::cout << "diff" << " " << i << " " << j << " " << buf1[j] << " " << buf2[j] << std::endl;
			}
		}
	}

	return 0;
}

#ifndef _LVDFILEREADER_H_
#define _LVDFILEREADER_H_

#include "config.h"
#include <pagefileinterface.h>
#include <lvdreader.h>
#include <plugin.h>


namespace ysl
{
	class LVDFileReader:public I3DBlockFilePluginInterface
	{
		DECLARE_RTTI
		//DECLARE_INITIAL(LVDFileReader)
		std::unique_ptr<LVDReader> lvdReader;
	public:
		void Open(const std::string& fileName)override
		{
			lvdReader = std::make_unique<LVDReader>(fileName);
			if(lvdReader == nullptr)
			{
				throw std::runtime_error("failed to open lvd file");
			}
		}
		const void * GetPage(size_t pageID) override { return lvdReader->ReadBlock(pageID); }
		size_t GetPageSize()const override { return lvdReader->BlockSize(); }
		size_t GetPhysicalPageCount()const override { return lvdReader->BlockCount(); }
		size_t GetVirtualPageCount()const override { return lvdReader->BlockCount(); }

		int GetPadding()const override { return lvdReader->GetBlockPadding(); }
		Size3 GetDataSizeWithoutPadding()const override{ return lvdReader->OriginalDataSize(); }
		Size3 Get3DPageSize()const override
		{
			const std::size_t len = lvdReader->BlockSize();
			return Size3{ len,len,len };	
		}
		int Get3DPageSizeInLog()const override{ return lvdReader->BlockSizeInLog(); }
		Size3 Get3DPageCount()const override{ return lvdReader->SizeByBlock(); }
	private:
		
	};

	class LVDFileReaderFactory:public IPluginFactory
	{
	public:
		DECLARE_PLUGIN_FACTORY("visualman.blockdata.io")
		std::vector<std::string> Keys() const override { return {".lvd"}; }
		std::unique_ptr<Object> Create(const std::string& key) override
		{
			if(key ==".lvd")
			{
				return std::make_unique<LVDFileReader>();
			}
			return nullptr;
		}
	};
	
}
EXPORT_PLUGIN_FACTORY(ysl::LVDFileReaderFactory)


#endif
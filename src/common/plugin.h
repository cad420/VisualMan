
#ifndef _PLUGIN_H_
#define _PLUGIN_H_
#include <vector>
#include <string>

#include "object.h"


#define DECLARE_PLUGIN_FACTORY(iid)\
	public:\
	std::string GetIID()const override{return iid;}

#define EXPORT_PLUGIN_FACTORY(pluginFactoryTypeName)\
	extern "C" COMMON_EXPORT_IMPORT ysl::IPluginFactory* GetPluginFactoryInstance();

#define EXPORT_PLUGIN_FACTORY_IMPLEMENT(pluginFactoryTypeName)\
	ysl::IPluginFactory* GetPluginFactoryInstance(){\
		static pluginFactoryTypeName factory;\
		return &factory;\
	}

#define DECLARE_PLUGIN_METADATA(pluginInterfaceTypeName,iid)\
	template<>\
	struct _iid_trait<pluginInterfaceTypeName>{\
		static const std::string GetIID(){return iid;}\
	};

template<typename T>
struct _iid_trait;

namespace ysl
{

	class IPluginFactory
	{
	public:
		virtual std::vector<std::string> Keys()const = 0;
		virtual std::unique_ptr<Object> Create(const std::string& key) = 0;
		virtual std::string GetIID()const = 0;
		virtual ~IPluginFactory() = default;
	};
	
	using FuncType = IPluginFactory * (*)();
		
}
#endif
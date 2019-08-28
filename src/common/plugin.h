
#ifndef _PLUGIN_H_
#define _PLUGIN_H_
#include <vector>
#include <string>

#include "object.h"
#include "libraryloader.h"


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


	

	class PluginLoader final
	{
		std::unordered_map<std::string, std::vector<std::function<IPluginFactory*()>>> factories;
	public:

		template<typename T> static 
		std::unique_ptr<T> CreatePlugin(const std::string & key)
		{
			const auto& f = GetPluginLoader()->factories;
			std::cout << _iid_trait<T>::GetIID() << std::endl;
			auto iter = f.find(_iid_trait<T>::GetIID());
			if(iter == f.end())
			{
				return nullptr;
			}
			for(const auto & fptr:iter->second)
			{
				//if(fptr()->Keys())
				for(const auto & k:fptr()->Keys())
				{
					if(key == k)
					{
						auto p = fptr()->Create(key).release();
						return std::unique_ptr<T>(Object_Dynamic_Cast<T>(p));
					}
				}
			}
			return nullptr;
		}

		static PluginLoader * GetPluginLoader()
		{
			static PluginLoader pluginLoader;
			return &pluginLoader;
		}

		static void LoadPlugins(const std::string & directory)
		{
			LibraryReposity::GetLibraryRepo()->AddLibraries(directory);
			for(const auto & lib:LibraryReposity::GetLibraryRepo()->GetLibRepo())
			{
				void* sym = nullptr;
				if((sym = lib.second->Symbol("GetPluginFactoryInstance")) != nullptr)
				{
					const auto func = reinterpret_cast<FuncType>(sym);
					const auto iid = func()->GetIID();
					GetPluginLoader()->factories[iid].push_back(func);
				}
			}
		}
	private:
		PluginLoader() = default;
		
	};
	
}
#endif
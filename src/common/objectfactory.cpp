
#include "objectfactory.h"
#include "object.h"
namespace ysl
{

	bool ObjectFactory::RegisterClass(const std::string& uid, std::function<std::unique_ptr<Object>()> ctor)
	{
		if (constructors.find(uid) != constructors.end())
		{
			return false;
		}
		constructors[uid] = ctor;
		return true;
	}

	void ObjectFactory::UnregisterClass(const std::string& uid)
	{
		auto it = constructors.find(uid);
		if (it != constructors.end())
			constructors.erase(it);
	}

	std::unique_ptr<Object> ObjectFactory::CreateInstance_Implement(const std::string& uid)
	{
		auto it = constructors.find(uid);
		if (it == constructors.end())
			return nullptr;
		return it->second();
	}
}

ysl::IObjectFactory* GetObjectFactory()
{
	static ysl::ObjectFactory instance;
	return &instance;
}


#include "reflection.h"
#include <iostream>
#include <string>
#include <unordered_map>

namespace ysl{

	static std::map<std::string, ClassInfo>* classInfoMap = nullptr;

    bool Register(const ClassInfo & ci)
    {
        if(!classInfoMap)
        {
            classInfoMap = new std::map<std::string,ClassInfo>();
        }
		if (classInfoMap->find(ci.className) == classInfoMap->end())
		{
			auto p = classInfoMap->insert({ci.className,ci});
        }
		return true;
    }
	int RegistedCount()
	{
		return classInfoMap->size();
	}
	IMPLEMENT_CLASS(Reflectable,"reflectable");

	ClassInfo::ClassInfo(const std::string & className, Constructor constructor):
	className(className),
	constructor(constructor)
	{
		Register(*this);
	}

	Reflectable* ClassInfo::CreateObject() const
	{
		return constructor ? constructor() : nullptr;
	}

	Reflectable::Reflectable()
	{
		
	}

	Reflectable::~Reflectable()
	{
		
	}

	Reflectable* Reflectable::CreateObject(const std::string name)
	{
		// statically link internal class
		auto it = classInfoMap->find(name);
		if (it != classInfoMap->end())
			return it->second.CreateObject();
		return nullptr;
	}
}

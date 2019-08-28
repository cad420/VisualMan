#include "object.h"
#include <memory>
#include <unordered_map>


namespace ysl
{
	pImplRtti::pImplRtti(const std::string& rttiName, Rtti* pBase, ObjectCtorFunc ctor):rttiName(rttiName),pBase(pBase),ctor(ctor)
	{

	}

	/*
	 * Rtti Implement
	 */
	Rtti::Rtti(const std::string& rttiName, Rtti* pBase, ObjectCtorFunc ctor):pimpl(new pImplRtti(rttiName,pBase,ctor))
	{

	}

	Rtti::~Rtti()
	{
		delete pimpl;
	}

	std::string Rtti::ObjectName() const
	{
		return pimpl->rttiName;
	}

	bool Rtti::SameType(const Rtti& rtti) const
	{
		return &rtti == this;
	}

	bool Rtti::operator==(const Rtti& rtti) const
	{
		return this == &rtti;
	}

	bool Rtti::operator!=(const Rtti& rtti) const
	{
		return this != &rtti;
	}

	bool Rtti::DerivedFrom(const Rtti& rtti) const
	{
		auto p = this;
		while (!p->SameType(rtti))
		{
			if (p->pimpl->pBase)
				p = p->pimpl->pBase;
			else
				return false;
		}
		return true;
	}

	Rtti* Rtti::GetBase() const
	{
		return pimpl->pBase;
	}

	std::shared_ptr<Object> Object::CreateObject(const std::string& name)
	{
		auto factory = GetObjectFactory();
		if (factory)
		{
			return factory->CreateInstance<Object>(name);
		}
		return nullptr;
	}
	

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


	//std::unique_ptr<std::unordered_map<std::string, ObjectCtorFunc>> Object::ms_pClassFactory = nullptr;
	IMPLEMENT_RTTI_NoParent(Object)
}

ysl::IObjectFactory* GetObjectFactory()
{
	static ysl::ObjectFactory instance;
	return &instance;
}


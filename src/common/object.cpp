#include "object.h"
#include <memory>
#include <unordered_map>

namespace ysl {

	std::unique_ptr<std::unordered_map<std::string, ObjectCtorFunc>> Object::ms_pClassFactory = nullptr;
	IMPLEMENT_RTTI_NoParent(Object)


		Rtti::Rtti(const std::string& rttiName, Rtti* pBase, ObjectCtorFunc ctor) :
		rttiName(rttiName),
		pBase(pBase),
		ctor(ctor)
	{

	}

	Rtti::~Rtti()
	{

	}

	std::string Rtti::ObjectName() const
	{
		return rttiName;
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
			if (p->pBase)
				p = p->pBase;
			else
				return false;
		}
		return true;
	}

	Rtti* Rtti::GetBase() const
	{
		return pBase;
	}
}

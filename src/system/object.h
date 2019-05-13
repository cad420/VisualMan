#pragma once
#include <string>
#include <functional>
#include <unordered_map>

#include "property.h"

class Object;
using ObjectCtorFunc = std::unique_ptr<Object>(*)();

class Rtti final
{

public:
	Rtti(const std::string & rttiName, Rtti * pBase, ObjectCtorFunc ctor);
	~Rtti();
	std::string ObjectName() const;
	bool SameType(const Rtti& rtti) const;
	bool operator==(const Rtti & rtti)const;
	bool operator!=(const Rtti & rtti)const;
	bool DerivedFrom(const Rtti& rtti)const;
	Rtti* GetBase() const;
private:
	std::string rttiName;
	Rtti * pBase;
	ObjectCtorFunc ctor;
	//std::vector<std::unique_ptr<Property>> properties;
};


#define DECLARE_RTTI																					\
	public:																								\
	static Rtti _ms_RttiType;																			\
	virtual Rtti & GetRtti()const{return _ms_RttiType;}		


#define IMPLEMENT_RTTI_NoConstructor(classTypeName,baseClassTypeName)									\
	Rtti classTypeName::_ms_RttiType(#classTypeName,&baseClassTypeName::_ms_RttiType,nullptr);

#define IMPLEMENT_RTTI_NoParent(classTypeName)															\
	Rtti classTypeName::_ms_RttiType(#classTypeName,nullptr,nullptr);

#define DECLARE_INITIAL(classTypeName)															\
	public:																								\
		static std::unique_ptr<Object> FactoryFunc();													\
		static bool __ms_registered;																	\
		static __init__dummy<classTypeName> __init_;

#define IMPLEMENT_INITIAL(classTypeName)															\
	bool classTypeName::__ms_registered = false;														\
	__init__dummy<classTypeName> classTypeName::__init_(#classTypeName,classTypeName::FactoryFunc);		\
	std::unique_ptr<Object> classTypeName::FactoryFunc()												\
	{																									\
		return std::make_unique<classTypeName>();														\
	}



/**
	 * \brief The class is all classes that need to be managed.
	 *
	 * If you should get rtti for some class, it should derived from the Object.
	 *
	 */

class Object:public std::enable_shared_from_this<Object>
{
protected:
	Object();
public:
	virtual std::string ToString()const;
	virtual ~Object();
	static std::unique_ptr<Object> CreateObject(const std::string& name)
	{
		if (ms_pClassFactory)
		{
			const auto iter = ms_pClassFactory->find(name);
			if (iter != ms_pClassFactory->end())
				return (*iter->second)();
		}
		return nullptr;
	}

	template<typename Ty> friend class __init__dummy;
	DECLARE_RTTI
protected:

	/**
	 * \brief This is a static pointer variable
	 *
	 * \note
	 * The initialization of the factory map should be in the charge of our control by delayed
	 * instantiation so that it is initialized before its first use.
	 * Because the initialized order of normal static variables in cpp is not defined, but they
	 * are guaranteed to be initialized before main()
	 */

	static std::unique_ptr<std::unordered_map<std::string, ObjectCtorFunc>> ms_pClassFactory;
};


inline 
Object::Object()
{
	
}

inline 
Object::~Object()
{
	
}

inline 
std::string Object::ToString() const
{
	return GetRtti().ObjectName();
}



/**
* \brief This class is used to execute some initialization before main()
*/

template<typename Ty>
class __init__dummy
{
public:
	__init__dummy(const std::string & name, ObjectCtorFunc ctor)
	{
		if (Object::ms_pClassFactory == nullptr)		// thread-unsafe
		{
			Object::ms_pClassFactory = std::make_unique<std::unordered_map<std::string, ObjectCtorFunc>>();
		}
		const auto iter = Object::ms_pClassFactory->find(name);
		if (iter == Object::ms_pClassFactory->end())
		{
			(*Object::ms_pClassFactory)[name] = ctor;
			Ty::__ms_registered = true;
		}
	}
};

template<typename Ty>
Ty* Object_Dynamic_Cast(Object * obj)
{
	if (obj == nullptr)
		return nullptr;
	return Ty::_ms_RttiType == obj->GetRtti() ? (Ty*)(obj) : nullptr;
}

template<typename Ty>
const Ty* Object_Dynamic_Cast(const Object * obj)
{
	if (obj == nullptr)
		return nullptr;
	return Ty::_ms_RttiType == (obj->GetRtti()) ? (Ty*)(obj) : nullptr;
}

template<typename Ty>
Ty * Object_Static_Cast(Object * obj)
{
	return static_cast<Ty>(obj);
}

template<typename Ty>
const Ty * Object_Static_Cast(const Object * obj)
{
	return static_cast<Ty>(obj);
}

//template<typename Ty>

template<typename Ty>
std::shared_ptr<Ty> Shared_Object_Dynamic_Cast(const std::shared_ptr<Object> &obj)
{
	if (obj == nullptr)
		return nullptr;
	return Ty::_ms_RttiType == obj->GetRtti() ? std::static_pointer_cast<Ty>(obj) : nullptr;
}

template<typename Ty>
std::shared_ptr<Ty> Shared_Object_Static_Cast(const std::shared_ptr<Object> & obj)
{
	return std::static_pointer_cast<Ty>(obj);
}
#pragma once
#include <string>
#include <functional>
#include "common.h"

#include "objectfactory.h"


namespace ysl
{

	class Object;
	using ObjectCtorFunc = std::function<std::unique_ptr<Object>()>;
	//using ObjectCtorFunc = std::unique_ptr<Object>(*)();

	//IObjectFactory* GetObjectFactory();

	/**
	 * \brief Rtti
	 */
	class COMMON_EXPORT_IMPORT Rtti final
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
	};


#define DECLARE_RTTI																					\
	public:																								\
	static Rtti _ms_RttiType;																			\
	virtual Rtti & GetRtti()const{return _ms_RttiType;}	


#define IMPLEMENT_RTTI_NoConstructor(classTypeName,baseClassTypeName)									\
	Rtti classTypeName::_ms_RttiType(#classTypeName,&baseClassTypeName::_ms_RttiType,nullptr);

#define IMPLEMENT_RTTI_NoParent(classTypeName)															\
	Rtti classTypeName::_ms_RttiType(#classTypeName,nullptr,nullptr);

#define DECLARE_INITIAL(classTypeName)																	\
	public:																								\
		static std::unique_ptr<Object> FactoryFunc();													\
		static bool __ms_registered;																	\
		static __init__dummy<classTypeName> __init_;

#define IMPLEMENT_INITIAL(classTypeName,classUID)														\
	bool classTypeName::__ms_registered = false;														\
	__init__dummy<classTypeName> classTypeName::__init_(#classUID,classTypeName::FactoryFunc);		    \
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

	class COMMON_EXPORT_IMPORT Object :public std::enable_shared_from_this<Object>
	{

	protected:
		Object() = default;
	public:
		virtual std::string ToString()const { return GetRtti().ObjectName(); }
		virtual ~Object() = default;
		template<typename Ty>
		std::shared_ptr<Ty> As() { return Shared_Object_Dynamic_Cast<Ty>(shared_from_this()); }

		static std::unique_ptr<Object> CreateObject(const std::string& name);
		template<typename Ty>
		static std::unique_ptr<Ty> CreateObject(const std::string & name);

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

		 //static std::unique_ptr<std::unordered_map<std::string, ObjectCtorFunc>> ms_pClassFactory;			// Only one in global
	};

	template <typename Ty>
	std::unique_ptr<Ty> Object::CreateObject(const std::string& name)
	{
		auto factory = GetObjectFactory();
		if (factory)
		{
			return factory->CreateInstance<Ty>(name);
		}
		return nullptr;
	}

	/**
	* \brief This class is used to execute some initialization before main()
	*/

	template<typename Ty>
	class __init__dummy
	{
	public:
		__init__dummy(const std::string & uid, ObjectCtorFunc ctor)
		{
			auto factory = static_cast<ObjectFactory*>(GetObjectFactory());
			factory->RegisterClass(uid, ctor);
			Ty::__ms_registered = true;
			//if (iter == Object::ms_pClassFactory->end())
			//{
			//	(*Object::ms_pClassFactory)[name] = ctor;
			//	Ty::__ms_registered = true;
			//}
		}
	};


	template<typename Ty>
	Ty* Object_Dynamic_Cast(Object * obj)
	{
		if (obj == nullptr)
			return nullptr;
		return Ty::_ms_RttiType.DerivedFrom(obj->GetRtti()) || obj->GetRtti().DerivedFrom(Ty::_ms_RttiType) ? static_cast<Ty*>(obj) : nullptr;
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


}


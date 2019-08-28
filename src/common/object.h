#pragma once
#include <string>
#include <functional>
#include "common.h"



namespace ysl
{

	class Object;
	using ObjectCtorFunc = std::function<std::unique_ptr<Object>()>;

	/**
	 * \brief Rtti
	 */

	class Rtti;
	
	struct pImplRtti
	{
		std::string rttiName;
		Rtti* pBase;
		ObjectCtorFunc ctor;
		pImplRtti(const std::string& rttiName, Rtti* pBase, ObjectCtorFunc ctor);
	};
	
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
		//std::string rttiName;
		//Rtti * pBase = nullptr;
		//ObjectCtorFunc ctor;
		pImplRtti* const pimpl;
	};

	
	struct IObjectFactory
	{
		virtual ~IObjectFactory() = default;
		template<typename T>
		std::shared_ptr<T> CreateInstance(const std::string & uid)
		{
			//auto ptr = CreateInstance_Implement(uid).release();
			//auto pp = Object_Dynamic_Cast<T>(ptr);
			//return std::unique_ptr<T>(pp);
			return Shared_Object_Dynamic_Cast<T>(CreateInstance_Implement(uid));
		}
	protected:
		virtual std::unique_ptr<Object> CreateInstance_Implement(const std::string & uid) = 0;
	};

	class COMMON_EXPORT_IMPORT ObjectFactory :public IObjectFactory
	{
	public:
		ObjectFactory() = default;
		ObjectFactory(const ObjectFactory &) = delete;
		ObjectFactory & operator=(const ObjectFactory &) = delete;
		virtual ~ObjectFactory() = default;
		bool RegisterClass(const std::string& uid, std::function<std::unique_ptr<Object>()> ctor);
		void UnregisterClass(const std::string& uid);
	protected:
		std::unique_ptr<Object> CreateInstance_Implement(const std::string& uid) override;
	private:
		std::unordered_map<std::string, std::function<std::unique_ptr<Object>()>> constructors;
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
		static std::shared_ptr<Object> CreateObject(const std::string& name);
		template<typename Ty>
		static std::shared_ptr<Ty> CreateObject(const std::string& name);
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
	std::shared_ptr<Ty> Object::CreateObject(const std::string& name)
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
		return Ty::_ms_RttiType.DerivedFrom(obj->GetRtti()) ||(obj->GetRtti().DerivedFrom(Ty::_ms_Rtti)) ? static_cast<Ty*>(obj) : nullptr;
	}

	template<typename Ty>
	Ty * Object_Static_Cast(Object * obj)
	{
		return static_cast<Ty>(obj);
	}

	template<typename Ty>
	const Ty * Object_Static_Cast(const Object * obj)
	{
		return static_cast<const Ty*>(obj);
	}
	
	template<typename Ty>
	std::shared_ptr<Ty> Shared_Object_Dynamic_Cast(const std::shared_ptr<Object> &obj)
	{
		return Object_Dynamic_Cast<Ty>(obj.get()) ? std::static_pointer_cast<Ty>(obj) : nullptr;
	}

	template<typename Ty>
	std::shared_ptr<Ty> Shared_Object_Static_Cast(const std::shared_ptr<Object> & obj)
	{
		return std::static_pointer_cast<Ty>(obj);
	}

}

#ifdef __cplusplus
extern "C"
{
#endif
	COMMON_EXPORT_IMPORT ysl::IObjectFactory * GetObjectFactory();
#ifdef __cplusplus
}
#endif


#ifndef _REFLECTION_H_

#include <map>


#define DECLARE_CLASS(className)						\
        protected:										\
             static ClassInfo class_info_;				\
        public:											\
             static Reflectable * CreateObject();


/*
* The key of IMPLEMENT_CLASS is to register the mapping relationship between name string and function pointer
* We can make use of the feature that any static variable will be constructed before main()
*/

#define IMPLEMENT_CLASS(className)																		\
        ClassInfo className::class_info_(#className,&className::CreateObject);							\
        Reflectable * className::CreateObject()															\
		{																								\
			return new className;																		\
		}


#define EXPORT_CONSTRUCTOR(BaseClassName,ObjectName,DerivedClassName,ExternalName)						\
	extern "C" __declspec(dllexport)																	\
	BaseClassName *create_##ObjectName##_##ExternalName()												\
	{																									\
		auto *instance = new DerivedClassName;															\
		return instance;																				\
	}																									\

namespace ysl
{
	class Reflectable;

	class ClassInfo
	{
	public:
		//using Constructor = Reflectable (*)(void);
		typedef Reflectable* (*Constructor)();
		std::string className;
		Constructor constructor;
	public:
		ClassInfo() = default;
		ClassInfo(const std::string& className, Constructor constructor);
		Reflectable * CreateObject()const;
	};

	class Reflectable
	{
		DECLARE_CLASS(Reflectable);
	public:
		Reflectable();
		virtual ~Reflectable();
		static Reflectable * CreateObject(const std::string name);
	};

	bool Register(const ClassInfo & ci);
	int RegistedCount();

} // nae ysl

#endif
#ifndef _REFLECTION_H_

#include <functional>
#include <map>
#include <memory>
namespace ysl
{

    // A naive reflection system
    // ...
    // Base * CreateObject(const std::string & class_name){
        // if(class_name == "A")return new A();
        // if(class_name == "B")return new B();
    // }
    // ...


    #define DECLARE_CLASS(className)         \
        protected:                           \
             static ClassInfo class_info_;   \
        public:                              \
             static Reflectable * CreateObject();


    /*
    * The key of IMPLEMENT_CLASS is to register the mapping relationship between name string and function pointer
    * We can make use of the feature that any static variable will be constructed before main()
    */

    #define IMPLEMENT_CLASS(className)                                            \
        ClassInfo className::class_info_(#className,(ClassInfo::Constructor)className::CreateObject);     \
        Reflectable * className::CreateObject(){                                  \
            return new className;                                                 \
        }


   class ClassInfo
    {
        public:
        using Constructor = std::function<Reflectable*(void)>;
        const std::string className;
        Constructor constructor;
        public:
        ClassInfo(const std::string & className,Constructor constructor):
        className(className),
        constructor(constructor)
        {
        }
        ClassInfo(const ClassInfo & ci){

        }

    };


    class Reflectable
    {
        DECLARE_CLASS(Reflectable);
        public:
        Reflectable(){}
        virtual ~Reflectable(){}
        static Reflectable * CreateObject(const std::string name);
    };


    bool Register(const ClassInfo & ci);

    
    template <typename ClassName>
    class CountedObject {
    public:
        std:size_t ObjectCount()const;
        ~CountedObject();
    protected:
        CountedObject();
        CountedObject(const CountedObject & obj);
    private:
        static std::size_t objectNum;
        void init();
    };

} // nae ysl

#endif
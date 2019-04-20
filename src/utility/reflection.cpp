
#include "reflection.h"

namespace ysl{

    static std::unique_ptr<std::map<std::string,ClassInfo>> classInfoMap;

    bool Register(const ClassInfo & ci)
    {
        if(!classInfoMap)
            classInfoMap = std::make_unique<std::map<std::string,ClassInfo>>();
        if(classInfoMap->find(ci.className) == classInfoMap->end()){
            (*classInfoMap)[ci.className] = ci;
        }
    }

    IMPLEMENT_CLASS(Reflectable);



    std::size_t CountedObject::objectNum = 0;

    void CountedObject::init()
    {
        objectNum++;
    }

    void CountedObject::~CountedObject()
    {
        objectNum--;
    }

    void CountedObject::CountedObject()
    {
        init();
    }

    void CountedObject::CountedObject(const CountedObject &obj)
    {
        init();
    }

    std::size_t CountedObject::ObjectCount() const
    {
        return objectNum;
    }


}
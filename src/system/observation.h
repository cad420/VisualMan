#pragma once

#ifndef _OBSERVATION_H_
#define _OBSERVATION_H_
#include <memory>
#include <set>
#include "object.h"

class Publisher;
class Subscriber;

class Subscriber:public Object
{

public:
	Subscriber() = default;
	virtual void Changed(std::weak_ptr<Publisher> pub)
	{
		
	}

	virtual  ~Subscriber() = default;
	
private:
	DECLARE_RTTI
	DECLARE_INITIAL(Subscriber)
};

IMPLEMENT_RTTI_NoConstructor(Subscriber,Object)
IMPLEMENT_INITIAL(Subscriber)

class Publisher:public Object
{

public:
	enum class NotifyType
	{
		Direct
	};
	Publisher() = default;
	virtual ~Publisher() = default;

	void Notify(NotifyType type = NotifyType::Direct)
	{
		for(auto & each:subs)
		{
			if (auto p = each.lock())
			{
				p->Changed(Shared_Object_Static_Cast<Publisher>(shared_from_this()));
			}
		}
	}

	void Notify(const std::string & objectName, NotifyType type = NotifyType::Direct)
	{
		for (auto & each : subs)
		{
			if(auto p = each.lock())
			{
				if (p->GetRtti().ObjectName() == objectName)
					p->Changed(Shared_Object_Static_Cast<Publisher>(shared_from_this()));
			}
		}
	}
	void Notify(std::weak_ptr<Subscriber> sub, NotifyType type = NotifyType::Direct)
	{
		if (auto p = sub.lock())
		{
			p->Changed(Shared_Object_Static_Cast<Publisher>(shared_from_this()));
		}
	}

	void Register(std::shared_ptr<Subscriber> sub)
	{
		subs.insert(sub);
	}

	void Unregister(std::shared_ptr<Subscriber> sub)
	{
		for(auto it = subs.begin();it != subs.end();++it)
		{
			if(it->expired() || it->lock().get() == sub.get())
			{
				it = subs.erase(it);
				if (it == subs.end())
					break;
			}
		}
	}

	//~Publisher(){}

	template<typename Ty>
	struct WeakPtrPred
	{
		bool operator()(const std::weak_ptr<Ty> & a,const std::weak_ptr<Ty> & b)const
		{
			return &a < &b;
		}
	};
private:
	std::set<std::weak_ptr<Subscriber>,WeakPtrPred<Subscriber>> subs;

	DECLARE_RTTI
	DECLARE_INITIAL(Publisher)
};

IMPLEMENT_RTTI_NoConstructor(Publisher, Object)
IMPLEMENT_INITIAL(Publisher)

#endif

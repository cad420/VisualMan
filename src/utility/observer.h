
#ifndef _OBSERVER_H_
#define _OBSERVER_H_
#include "countedobject.h"
#include <string>
#include <set>


namespace ysl
{
	class Observer:public CountedObject<Observer>
	{
	public:
		Observer();
		void Notify();
		virtual void Changed(Observer * obs);
		void RegisterObserver(Observer * obs);
		void UnregisterObserver(Observer * obs);
		virtual  ~Observer();
		std::string ToString()const;
	private:
		std::set<Observer*> observers;
	};

}

#endif

#include "observer.h"

namespace ysl
{
	Observer::Observer()
	{

	}

	void Observer::Notify()
	{
		for (auto obs : observers)
			obs->Changed(this);
	}

	void Observer::Changed(Observer* obs)
	{
		(void)obs;
	}

	void Observer::RegisterObserver(Observer* obs)
	{
		observers.insert(obs);
	}

	void Observer::UnregisterObserver(Observer* obs)
	{
		observers.erase(obs);
	}

	Observer::~Observer()
	{
		for(auto item:observers)
		{
			item->observers.erase(this);
		}
	}

	std::string Observer::ToString() const
	{
		return "ysl::Observer";
	}
}

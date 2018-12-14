#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>
#include <iostream>

class Timer
{
	std::chrono::time_point<std::chrono::system_clock> begin_, end_;
	std::chrono::microseconds duration_;
public:
	Timer() {};
	void begin()
	{
		begin_ = std::chrono::system_clock::now();
	}
	void end()
	{
		end_ = std::chrono::system_clock::now();
		duration_ = std::chrono::duration_cast<std::chrono::microseconds>(end_ - begin_);
	}
	void print()const
	{
		std::cout << "Duration:" << duration_.count() << "ms.\n";
	}
};
#endif

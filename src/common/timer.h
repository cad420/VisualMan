#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>
#include <iostream>

class Timer
{
	std::chrono::time_point<std::chrono::system_clock> begin_, end_;
	std::chrono::microseconds duration_;
public:
	Timer() = default;
	void start()
	{
		begin_ = std::chrono::system_clock::now();
	}
	void stop()
	{
		end_ = std::chrono::system_clock::now();
		duration_ = std::chrono::duration_cast<std::chrono::microseconds>(end_ - begin_);
	}

	auto last_begin_time_point()const { return begin_; }
	void print()const
	{
		std::cout << "Duration:" << duration_.count() << "ms.\n";
	}
	auto duration()const	{return duration_.count();}
	auto elapse()const	{return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - begin_).count();}
	double eval_remaining_time(float cur_percent)const { return 1.0*elapse() * (1.0 - cur_percent) / (cur_percent); }
	auto eval_total_time(float cur_percent)const { return elapse() / cur_percent; }
	auto elapse_second()const { return elapse()*1.0 / 1000000; }
	double duration_to_seconds()const	{return duration()*1.0 / 1000000;}
};
#endif

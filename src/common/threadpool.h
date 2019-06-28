
#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include "common.h"
#include <thread>
#include <condition_variable>
#include <queue>
#include <vector>
#include <atomic>
#include <mutex>

namespace ysl
{
	class COMMON_EXPORT_IMPORT ThreadArena
	{
	public:
		using Task = std::function<void()>;
		ThreadArena(int tNum) :threadNum(tNum),running(false) {}
		ThreadArena(const ThreadArena &) = delete;
		ThreadArena & operator=(const ThreadArena&) = delete;
		void Start();
		void Stop();

		void AppendTask(const Task& t)
		{
			if (running)
			{
				std::unique_lock<std::mutex> lk(mut);
				tasks.push(t);
				cond.notify_one();
			}
		}

		~ThreadArena()
		{
			if (running)
				Stop();
		}

	private:
		void Worker()
		{
			while (running)
			{
				Task t;
				{
					std::unique_lock<std::mutex> lk(mut);
					if (!tasks.empty()) {
						t = tasks.front();
						tasks.pop();
					}
					else if (running && tasks.empty())
					{
						cond.wait(lk);
					}
				}
				if (t)
					t();
			}
		}
		int threadNum = 1;
		std::atomic_bool running = false;
		std::vector<std::thread> threads;
		std::queue<Task> tasks;
		std::mutex mut;
		std::condition_variable cond;
	};

	inline void ThreadArena::Start()
	{
		if (running == false)
		{
			running = true;
			std::unique_lock<std::mutex> lk(mut);
			for (int i = 0; i < threadNum; i++)
			{
				threads.emplace_back(std::thread(&ThreadArena::Worker, this));
			}
		}
	}

	inline void ThreadArena::Stop()
	{

		{
			std::unique_lock<std::mutex> lk(mut);
			running = false;
			cond.notify_all();		// Wake up all threads to finish their works
		}

		// Waiting all threads to be done before stop
		for (auto & t : threads)
			if (t.joinable())
				t.join();

	}

}
#endif

#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include "common.h"
#include <thread>
#include <condition_variable>
#include <queue>
#include <vector>
#include <atomic>
#include <mutex>
#include <future>

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

		void AppendTask(const Task & t)
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


	class ThreadPool 
	{
	public:
		ThreadPool(size_t);
		template<class F, class... Args>
		auto AppendTask(F&& f, Args&&... args);
		~ThreadPool();
	private:
		std::vector<std::thread> workers;
		std::queue<std::function<void()>> tasks;
		std::mutex mut;
		std::condition_variable cond;
		bool stop;
	};

	// the constructor just launches some amount of workers
	inline ThreadPool::ThreadPool(size_t threads)
		: stop(false)
	{
		for (size_t i = 0; i < threads; ++i)
			workers.emplace_back(
				[this]
				{
					for (;;)
					{
						std::function<void()> task;

						{
							std::unique_lock<std::mutex> lock(this->mut);
							this->cond.wait(lock,
								[this] { return this->stop || !this->tasks.empty(); });
							if (this->stop && this->tasks.empty())
								return;
							task = std::move(this->tasks.front());
							this->tasks.pop();
						}

						task();
					}
				}
				);
	}

	// add new work item to the pool
	template<class F, class... Args>
	auto ThreadPool::AppendTask(F&& f, Args&&... args)
	{
		using return_type = std::invoke_result_t<F,Args...>;
		auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
		std::future<return_type> res = task->get_future();
		{
			std::unique_lock<std::mutex> lock(mut);
			// don't allow enqueueing after stopping the pool
			if (stop)
				throw std::runtime_error("enqueue on stopped ThreadPool");
			tasks.emplace([task]() { (*task)(); });
		}
		cond.notify_one();
		return res;
	}
	// the destructor joins all threads
	inline ThreadPool::~ThreadPool()
	{
		{
			std::unique_lock<std::mutex> lock(mut);
			stop = true;
		}
		cond.notify_all();
		for (std::thread &worker : workers)
			worker.join();
	}
}
#endif
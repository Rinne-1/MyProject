#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t n) : stop(false)
{
	for (size_t i = 0; i < n; i++)
	{
		workers.emplace_back([this]()
			{
				while (true)
				{
					function<void()> task;
					{
						unique_lock<mutex> lock(queueMutex);
						condition.wait(lock, [this]() { return stop || !tasks.empty(); });
						if (stop && tasks.empty()) return;
						task = move(tasks.front());
						tasks.pop();
					}
					task();
				}
			});
	}
}

ThreadPool::~ThreadPool()
{
	{
		unique_lock<mutex> lock(queueMutex);
		stop = true;
	}
	condition.notify_all();
	for (thread& worker : workers)
		worker.join();
}

void ThreadPool::enqueue(function<void()> task)
{
	{
		unique_lock<mutex> lock(queueMutex);
		tasks.push(move(task));
	}
	condition.notify_one();
}
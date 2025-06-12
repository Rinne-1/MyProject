#pragma once
#include "DatabaseManager.h"


class ThreadPool
{
private:
	vector<thread> workers;
	queue<function<void()>> tasks;
	mutex queueMutex;
	condition_variable condition;
	bool stop;

public:
	ThreadPool(size_t n);
	~ThreadPool();
	void enqueue(function<void()> task);
};


#pragma once

#include <queue>
#include <functional>

class TaskScheduler
{
	typedef std::function<void()> task_t;
	std::queue<task_t> tasks;
public:
	void add(const task_t& t) {
		tasks.push(t);
	}
	void start();
	std::function<void(int,int)> onprogress = NULL;
};


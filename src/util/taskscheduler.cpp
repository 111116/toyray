#include <vector>
#include <mutex>
#include <thread>
#include "lib/consolelog.hpp"
#include "taskscheduler.hpp"

void TaskScheduler::start()
{
	int total = tasks.size();
	int finished = 0;
#ifndef SINGLE_THREADED
	int n = std::thread::hardware_concurrency();
	console.info("Scheduler: Starting with", n, "threads");
	std::vector<std::thread> threads;
	std::mutex ex,ex2;
	auto work = [&](){
		try {
			while (!tasks.empty()) {
				ex.lock();
				task_t task = tasks.front();
				tasks.pop();
				ex.unlock();
				task();
				if (onprogress) {
					ex2.lock();
					finished += 1;
					onprogress(finished, total);
					ex2.unlock();
				}
			}
		}
		catch (const char* err) {
			console.error(err);
			throw err;
		}
	};
	for (int i=0; i<n; ++i)
		threads.push_back(std::thread(work));
	for (int i=0; i<n; ++i)
		threads[i].join();
#else
	console.warn("Scheduler: Starting single threaded");
	while (!tasks.empty()) {
		task_t task = tasks.front();
		tasks.pop();
		task();
		if (onprogress) {
			finished += 1;
			onprogress(finished, total);
		}
	}
#endif
}

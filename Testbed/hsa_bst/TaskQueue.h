


#ifndef _TASKQUEUE_H_
#define _TASKQUEUE_H_

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "Task.h"

class TaskQueue {

	class Worker {
	public:
		Worker(TaskQueue &tq, size_t id);
		void operator() ();

	private:


		TaskQueue& taskQueue;

		size_t id;

	};


public:
	TaskQueue();
	~TaskQueue();

//	void createWorkers();

	void enqueue(Task*);

private:


	// Keep track of Worker threads
	std::vector<std::thread> workerThreads;
	size_t numThreads;

	// the task queue
	std::queue<Task*> tasks;

	std::mutex queueMutex;
	std::condition_variable condition;

	volatile bool stop;

};




#endif

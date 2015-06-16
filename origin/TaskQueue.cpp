
//debug
#include <iostream>

#include "TaskQueue.h"


TaskQueue::Worker::Worker(TaskQueue& tq, size_t threadID) :
taskQueue(tq), id(threadID) {
//	std::cerr << "Init worker" << std::endl;
}


void TaskQueue::Worker::operator() () {


	while (true) {


		std::unique_lock<std::mutex> lock(taskQueue.queueMutex);

		while (!taskQueue.stop && taskQueue.tasks.empty())
			taskQueue.condition.wait(lock);

		// setup

		if (taskQueue.stop) {
			lock.unlock();
			return;
		}

//		std::cerr << "Work" <<std::endl;

		Task* taskInstancePtr = taskQueue.tasks.front();
		Event& eventInstanceRef = taskInstancePtr->taskEvent;

		// Take a task
//		eventInstanceRef.increment();

//		Task task(*taskInstancePtr);
//

		size_t countIndex = taskInstancePtr->chunkCount;
		++(taskInstancePtr->chunkCount);


//		std::cerr << taskInstancePtr->chunkCount << " ID: " << this->id << std::endl;


		// atomic ?
		if (taskInstancePtr->chunkCount == taskInstancePtr->numChunk) {
//			std::cerr << "POP: " << taskInstancePtr->chunkCount << "ID: [" << this->id << "]" <<std::endl;

//			eventInstanceRef.decrement(); // to pair with enqueue
			taskQueue.tasks.pop();

//			std::cerr << "POP2" <<std::endl;
		}

		lock.unlock();

		// Execute ?
		// thread ID ?

		// task execution
		taskInstancePtr->execute(countIndex, this->id, taskQueue.numThreads);

		// finished
//		eventInstanceRef.decrement();


		--taskInstancePtr->taskCompleted;
		if (taskInstancePtr->taskCompleted == 0) {
			eventInstanceRef.decrement();
		}


	}

}




TaskQueue::TaskQueue() :
stop(false), numThreads(5) {

	// Reference to Embree thread function
	// runfunction / Complete function
	// void TaskScheduler::threadFunction
	// use C++ 11 function and template
	// Worker

	std::cerr << "TaskQueue ctor" << std::endl;

	for (size_t i = 0; i < numThreads; ++i) {
		workerThreads.push_back(std::thread(Worker(*this, i)));
	}


}


//
//void TaskQueue::createWorkers() {
//
//
//	std::cerr << "TaskQueue createWorkers" << std::endl;
//
//	numThreads = 4;
//
//	for (size_t i = 0; i < numThreads; ++i) {
//		workerThreads.push_back(std::thread(Worker(*this)));
//	}
//}


void TaskQueue::enqueue(Task* tkPtr) {

	tkPtr->taskEvent.increment();

	{
		std::unique_lock<std::mutex> lock(queueMutex);
		tasks.push(tkPtr);
	}

	std::cerr << "TaskQueue::enqueue" << std::endl;

	condition.notify_all(); // all or one ?

	//condition.notify_one();
}

TaskQueue::~TaskQueue() {
	{
		std::unique_lock<std::mutex> lock(queueMutex);
		// do something
		// clean up

		stop = true;
	}

	std::cerr << "TaskQueue Exit" << std::endl;

	std::cerr << "thread pool size: " << workerThreads.size() << std::endl;

	condition.notify_all();
	for (auto &wk : workerThreads) {
		std::cerr << "Join" << std::endl;
		wk.join();
	}

	//for (size_t i = 0; i < workerThreads.size(); ++i) {
	//	workerThreads[i].join();
	//}

	std::cerr << "After Join" << std::endl;

}


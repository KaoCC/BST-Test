

#ifndef _TASK_H_
#define _TASK_H_

#include <string>

#include <functional>
#include <atomic>

#include "Event.h"
#include "Semaphore.h"


class Task {

public:

	Task(std::function<void(void*, int)>& runFunc, void* dataPtr, size_t ndr,
		const size_t elePerChunk, const std::string& taskName, Event& sem);

	virtual void execute(size_t countIndex, size_t threadIndex, const size_t threadCount);

	// Task Event

	// run function
	std::function<void(void* data, int gid)> runFunction;

	//data for runfunction
	void* runData;

	// complete function

	// data for complete function




	//SPMD style
	size_t NDRange;

	//element per chunk
	const size_t elementPerChunk;

	volatile size_t chunkCount;

	//total number of elements (chunks)
	const size_t numChunk;

	
	std::atomic<size_t> taskCompleted;

	// name of the task
	std::string name;

	// Event
	Event& taskEvent;
	

};




#endif

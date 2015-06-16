
#include "Task.h"
#include "EventSync.h"

#include <iostream>



Task::Task(std::function<void(void*, int)>& runFunc,
		void* dataPtr, size_t ndr, const size_t elePerChunk, const std::string& taskName, Event& ev)
	: runFunction(runFunc), runData(dataPtr), NDRange(ndr), elementPerChunk(elePerChunk),
	chunkCount(0), numChunk(ndr / elePerChunk), taskCompleted(ndr / elementPerChunk), name(taskName), taskEvent(ev) {

	}



void Task::execute(size_t countIndex, size_t threadID, const size_t threadCount) {

	// threadID & threadCount ?

	int startIndex = countIndex * elementPerChunk;
	int offsetSize = elementPerChunk;

	//check if it is the last chunk
	if (countIndex == this->numChunk - 1) {
		// take all the rest of the elements
		offsetSize = NDRange - startIndex;
	}

	// Execution
	for (int i = startIndex; i < startIndex + offsetSize; ++i) {
		runFunction(runData, i);
	}

}

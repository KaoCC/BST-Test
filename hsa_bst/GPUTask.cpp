

#include "GPUTask.h"


#include <iostream>

GPUTask::GPUTask(std::function<void(void*, int)>& runFunc, std::function<void(void*, int, size_t)>& gpuFunc,void* dataPtr, size_t ndr,
		const size_t elePerChunk, const std::string& taskName, Event& ev)
	: Task(runFunc, dataPtr, ndr, elePerChunk, taskName, ev), gpuFunction(gpuFunc) {

	}




void GPUTask::execute(size_t countIndex, size_t threadID, const size_t threadCount) {

	int startIndex = countIndex * elementPerChunk;
	int offsetSize = elementPerChunk;

	bool gpuThreadFlag = false;

	if (threadID == 0) {
		gpuThreadFlag = true;
	}

	if (countIndex == this->numChunk - 1) {
		offsetSize = NDRange - startIndex;
		gpuThreadFlag = false;
	}

//	gpuThreadFlag = true;

	if (gpuThreadFlag) {

		// GPU
		//

//		std::cerr << threadID << " " <<countIndex <<std::endl;

		gpuFunction(runData, startIndex, offsetSize);

	} else {
		// CPU
		//

		for (int i = startIndex; i < startIndex + offsetSize; ++i) {
			// SPMD-style
			runFunction(runData, i);
		}

	}


	
	
}



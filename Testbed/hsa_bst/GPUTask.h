


#ifndef _GPUTASK_H_
#define _GPUTASK_H_

#include "Task.h"

class GPUTask : public Task {

public:

	GPUTask(std::function<void (void*, int)>& runFunc, std::function<void (void*, int, size_t)>& gpuFunc, void* dataPtr, size_t ndr, 
	      const size_t elePerChunk, const std::string& taskName, Event& sem);

	void execute(size_t, size_t, const size_t) override;

private:

	std::function<void (void*, int index, size_t offset)> gpuFunction;

};








#endif






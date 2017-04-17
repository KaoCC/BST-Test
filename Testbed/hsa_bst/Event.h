

#ifndef _EVENT_H_
#define _EVENT_H_

#include <atomic>

#include "Semaphore.h"

class Event {

public:
	Event(int activeCount = 1) : active(activeCount) {

	}


	void increment() {
		++active;
	}

	void decrement() {
		--active;

		if (active == 0) {
			trigger();
		}
	}


	virtual void trigger() = 0;


private:
	std::atomic<int> active;
};




#endif

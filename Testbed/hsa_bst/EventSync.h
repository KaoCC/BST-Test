

#ifndef _EVENTSYNC_H_
#define _EVENTSYNC_H_

#include "Event.h"



class EventSync : public Event {

public:
	EventSync() :  Event(1) {
	}

	void sync() {
		this->decrement();
		sem.wait();
	}


	void trigger() override {
		sem.signal();
	}

private:

	Semaphore sem;
};


#endif



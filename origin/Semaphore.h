
#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include <mutex>
#include <condition_variable>


class Semaphore {

public:

	Semaphore(unsigned long long cnt = 0): count(cnt) {

	}

	void signal() {
		std::unique_lock<std::mutex> lock(mtx);
		++count;
		cv.notify_one();
	}

	void wait() {
		std::unique_lock<std::mutex> lock(mtx);

		while (count == 0) {
			cv.wait(lock);
		}

		--count;
	}


private:

	std::mutex mtx;
	std::condition_variable cv;
	unsigned long long count;

};


#endif
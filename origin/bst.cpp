
#include <cstdlib>

#include <iostream>

#include <vector>
#include <functional>
#include <string>

#include <limits>

#include <sys/time.h>

#include "TaskQueue.h"
#include "EventSync.h"

#include "GPUTask.h"

#include "kernel.h"

typedef int key_t;
typedef int value_t;


struct Data {
	int key;
	int value;
};



struct Node {
	Data data;

	Node* left;
	Node* right;

};


Node* Insert (Node* node, const Data& data) {
	if (!node) {
		Node* tmp = new Node();

//		std::cout << "Node Address: " << tmp << std::endl;

		tmp->data = data;
		tmp->left = nullptr;
		tmp->right = nullptr;

		return tmp;
	}


	if (data.key > (node->data.key)) {

		node->right = Insert(node->right, data);

	} else if (data.key < (node->data.key)) {
		node->left = Insert(node->left, data);
	} else {
		// duplicate ...
	}

	return node;
}

Node* Find (Node* node, key_t key) {

	if (!node) {
		// exception
		return nullptr;
	}

	if (key > node->data.key) {
		return Find(node->right, key);

	} else if (key < node->data.key) {
		return Find(node->left, key);

	} else {
		// element found
		return node;
	}

}



const int size = 4096 * 128;
Data a[size];
key_t b[size];
key_t b_tmp[size];
Node* tmp_serial[size];
Node* tmp_list[size];


void print_test(Node* test[]) {

	for (int i = 0; i < size; ++i) {
		if (test[i]) {
			std::cout << "value:" << test[i]->data.value << std::endl;
		} else {
			std::cout << "Not Found" <<std::endl;
		}
	}
}



//Time Measurement helper

typedef unsigned long long measure_t;

/*measure_t timeMeasurementHelper(std::function<void(Node*, Node*, int*, const snk_lparm_t)> func, Node* root, 
		Node* out, int* key_list, const snk_lparm_t& lparam, const std::string& str) {*/


measure_t timeMeasurementHelper(std::function<void()> func, const std::string& str) {

	struct timeval tv1, tv2;
	measure_t startTime = 0, endTime = 0;
	measure_t T = 0;

	gettimeofday(&tv1, NULL);
	startTime = tv1.tv_sec * 1000000 + tv1.tv_usec;


	func();

	gettimeofday(&tv2, NULL);
	endTime = tv2.tv_sec * 1000000 + tv2.tv_usec;

	T = endTime - startTime;
//	std::cout << str << " Time: " << T << std::endl;

	return T;

}


void taskQueueHelper(TaskQueue& tq, Task* taskPtr, EventSync& event) {
	tq.enqueue(taskPtr);
	event.sync();
}

//POD-style
struct TaskQueueData {
	Node* root;
	Node** result;
	key_t* key_ptr;
};

// SPMD-style function prototype, for Task Queue
void runFunction(void* t, int gid) {

	TaskQueueData* ptr = static_cast<TaskQueueData*>(t);

	// Now call the Find function
	Find(ptr->root, ptr->key_ptr[gid]);
	
}


void runSerial(Node* root) {
	for (int i = 0 ; i < size; ++i) {
		Find(root, b[i]);
	}
}


// for HSA-GPU
void gpuFunction(void* t, int index, size_t offset) {

	TaskQueueData* ptr = static_cast<TaskQueueData*>(t);

        SNK_INIT_LPARM(lparm, offset);

        //snk_lparm_t lparm = {.ndim = 1, .gdims = {offset}, .ldims = {256}};

	Find(ptr->root, &(ptr->result[index]), &(ptr->key_ptr[index]), lparm);


//	std::cerr << "index:" << index << std::endl;

}


int main () {


	//Data a[size];
	//key_t b[size];
	//


	for (int i = 0; i < size; ++i) {
		b[i] = rand() % size;
	}


	for (int i = 0; i < size; ++i) {
		a[i].key = b[i];
		a[i].value = i;
	}

	std::cout << "TEST START" << std::endl;

	Node* root = nullptr;

	std::cout << "Build Tree (Insert)" <<std::endl;
	for (int i = 0 ; i < size; ++i) {

		root = Insert(root, a[i]);
		//std::cout << "Insert" << "key: " << a[i].key << "value: " << a[i].value << std::endl;
	}

	std::cout << "Build Finished" <<std::endl;

	// TEST START
	//
	//
	

	//Node* tmp_serial[size];

	for (int i = 0; i < size; ++i) {
		tmp_serial[i] = nullptr;
	}

	struct timeval tv1, tv2;
	unsigned long long startTime = 0, endTime = 0;
	unsigned long long T1 = 0, T2 = 0;


	//AGAIN
	for (int i = 0; i < size; ++i) {
		b[i] = rand() % size;
		b_tmp[i] = b[i];
	}

	std::cout << "CPU start" <<std::endl;
	gettimeofday(&tv1, NULL);
	startTime = tv1.tv_sec * 1000000 + tv1.tv_usec;

	// serial
	for (int i = 0; i < size; ++i) {
		tmp_serial[i] = Find(root, b[i]);

	}

	gettimeofday(&tv2, NULL);
	endTime = tv2.tv_sec * 1000000 + tv2.tv_usec;

	T1 = endTime - startTime;
	std::cout << "CPU finished" << std::endl;


	// test
//	print_test(tmp_serial);

	// HSA
	//
	

//	int n;
//	std::cin >> n;	

	//Node* tmp_list[size];

	for (int i = 0; i < size; ++i) {
		tmp_list[i] = NULL;
	}
        
        SNK_INIT_LPARM(lparm, size);
            
	//snk_lparm_t lparm = {.ndim = 1, .gdims = {size}, .ldims = {256}};


	std::cout << "HSA start" << std::endl; 
	gettimeofday(&tv1, NULL);
	startTime = tv1.tv_sec * 1000000 + tv1.tv_usec;

	// Kernel
	Find(root, tmp_list, b, lparm);	

	gettimeofday(&tv2, NULL);
	endTime = tv2.tv_sec * 1000000 + tv2.tv_usec;

	T2 = endTime - startTime;
	std::cout << "HSA finished" <<std::endl;

	//test
//	print_test(tmp_list);

	std::cout << " Time Result: CPU:" << T1 << " GPU:" << T2 <<std::endl;

	
//	int t;
//	std::cin >> t;

	std::cout << "No Compilation Overhead Test" <<std::endl;


	std::cout << "HSA No Branch start" << std::endl; 
	gettimeofday(&tv1, NULL);
	startTime = tv1.tv_sec * 1000000 + tv1.tv_usec;

	// Kernel
	Find(root, tmp_list, b, lparm);	

	gettimeofday(&tv2, NULL);
	endTime = tv2.tv_sec * 1000000 + tv2.tv_usec;

	unsigned long long T3 = endTime - startTime;
	std::cout << "HSA No Compilation Overhead finished" <<std::endl;


	std::cout << "HSA No CO Time Result: " << T3 << std::endl;

	std::cout << "Remove Branch" << std::endl;

	for (int i = 0; i < size; ++i) {
		b[i] = size / 2;
	}

	
	gettimeofday(&tv1, NULL);
	startTime = tv1.tv_sec * 1000000 + tv1.tv_usec;

	// Kernel
	Find(root, tmp_list, b, lparm);	

	gettimeofday(&tv2, NULL);
	endTime = tv2.tv_sec * 1000000 + tv2.tv_usec;

	unsigned long long T4 = endTime - startTime;
	std::cout << "HSA no Branch finished" <<std::endl;


	std::cout << "HSA No branch Time Result: " << T4 << std::endl;



	//============================== Getting longer .....=========
	//
	//
	//
	//
	
	int t;
	std::cin >>t;

	std::cout << "======= Round 2 : ======" << std::endl;	


//	typedef measure_t unsigned long long;
	std::vector<measure_t> time_data;
	std::vector<key_t> rand_key;

	const int ROUND = 64*64;

	// Generate Key
	//

	std::cout << "Generate Key" <<std::endl;	
	for (int i = 0; i < ROUND; ++i) {
		key_t test_key = rand() % size;
		rand_key.push_back(test_key);
	}

	// CPU test
	//

	for (int i = 0; i < size; ++i) {
		b[i] = rand_key[i % ROUND];
	}


	std::cout << "CPU" <<std::endl;
	measure_t T_cpu_2 = timeMeasurementHelper(std::bind(runSerial, root), "CPU");

	std::cout << "GPU" <<std::endl;
	measure_t T_gpu_2 = timeMeasurementHelper(std::bind((void (*) (void*, void*, int*, const snk_lparm_t*))Find, root,
			      tmp_list, b, lparm), "GPU");

		

	// GPU No branch
	measure_t T_max = 0, T_min = std::numeric_limits<measure_t>::max(), T_tmp = 0;
	key_t longest_key = 0, shortest_key = 0;


	std::cout << "same key" << std::endl;
	for (int i = 0; i < ROUND ; ++i) {

//		key_t test_key = rand() % size;
//		rand_key.push_back(test_key);

		for (int j = 0; j < size; ++j) {
			// pick one value
			b[j] = rand_key[i];
		}


		//std::function<void(void*, void*, int*, snk_lparm_t)> f = (void (*) (void*, void*, int*, snk_lparm_t))Find;
			

		time_data.push_back( T_tmp = timeMeasurementHelper(
					std::bind((void (*) (void*, void*, int*, const snk_lparm_t*))Find, 
						root, tmp_list, b, lparm), "Round " + std::to_string(i)));

		if (T_tmp > T_max) {
			T_max = T_tmp;
			longest_key = rand_key[i];
		}

		if (T_tmp < T_min) {
			T_min = T_tmp;
			shortest_key = rand_key[i];
		}
		


	}

	measure_t avg = 0;
	for (auto& element : time_data) {
		avg += element;
	}

	avg /= time_data.size();

	std::cout << "Average Time:" <<avg <<std::endl;


	// 64-group key assignment	


	for (int i = 0; i < size; ++i) {

		b[i] = rand_key[(i / 64) % ROUND];
	}

	measure_t T_g = timeMeasurementHelper( std::bind((void (*) (void*, void*, int*, const snk_lparm_t*))Find, 
				root, tmp_list, b, lparm), "64-group");


	for (int i = 0; i < size; ++i) {
		b[i] = rand_key[i % ROUND];
	}

	measure_t T_g_tmp = timeMeasurementHelper( std::bind((void (*) (void*, void*, int*, const snk_lparm_t*))Find, 
				root, tmp_list, b, lparm), "64-group");




	// Task Queue
	//
	//
	
	for (int i = 0; i < size; ++i) {
		b[i] = b_tmp[i];

//		b[i] = rand_key[(i / 64) % ROUND];
	}


	TaskQueue tq;	
	EventSync event;


	TaskQueueData data;
	data.root = root;
	data.result = tmp_list;
	data.key_ptr = b;


	std::function<void(void*, int)> f = runFunction;

	std::cout << "TaskQueue" << std::endl;	
	Task task(f, &data, size, 1024, "SPMD" , event);

	//measure_t T_tq = 0;

//	tq.enqueue(task);
//	event.sync();

	measure_t T_tq = timeMeasurementHelper(std::bind(taskQueueHelper, std::ref(tq), &task, std::ref(event)), "TaskQueue");


	// GPU Task
	//
	

	std::function<void(void*, int, size_t)> gf = gpuFunction;

	EventSync gpuEvent;
	GPUTask gpuTask(f, gf, &data, size, size/512, "GPU", gpuEvent);

	Task* taskPtr = &gpuTask;

	measure_t T_tq_gpu = timeMeasurementHelper(std::bind(taskQueueHelper, std::ref(tq), &gpuTask, std::ref(gpuEvent)), "GPUTask");


	// Hybrid Task
	//
	

	

	//=========== RESULT Block ====================

	
	std::cout << "========== Result: ==========" << std::endl;

	std::cout << "CPU: " << T1 << std::endl;

	std::cout << "GPU (HSA) with CO: " << T2 <<std::endl;

	std::cout << "No CO: " << T3 << std::endl;

	std::cout << "Round 2" << std:: endl;

	std::cout << "CPU:" << T_cpu_2 << std::endl;

	std::cout << "GPU:" << T_gpu_2 << std::endl;

	std::cout << "T(Size/2): (Time, key)" << T4 << "," << (size / 2) <<std::endl;

	std::cout << "No branch:" << "AVG:" << avg << " (T_Min, key): " << T_min << "," << longest_key <<"(T_Max: key)" << T_max << "," << shortest_key <<std::endl;
	
	std::cout << "64-group: " << T_g <<std::endl;

	std::cout << "tmp: " << T_g_tmp << std::endl;

	std::cout << "CPU thread TaskQueue: " << T_tq <<std::endl;

	std::cout << "GPU/GPU thread pool:" << T_tq_gpu << std::endl;




//	print_test(tmp_list);

	return 0;
}





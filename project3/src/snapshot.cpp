/*
 * Multicore Programming
 * snapshot.cpp
 * author : Heejun Lee
 * Copyright (C) 2017 Heejun Lee
 * All rights reserved.
 */

#include "snapshot.h"
#include <iostream>

using namespace std;

int thread_num;
stamp_snap * a_table;
stamp_snap ** _copy;
bool ** moved;
long ** result;

void wait_free_initialize(long start){
	a_table = (stamp_snap*)malloc(sizeof(stamp_snap) * thread_num);
	_copy = (stamp_snap**)malloc(sizeof(stamp_snap*) * thread_num);
	moved = (bool**)malloc(sizeof(bool*) * thread_num);
	result = (long**)malloc(sizeof(long*) * thread_num);

	for(int i=0;i<thread_num; i++){
		a_table[i].snap = (long*)malloc(sizeof(long)*thread_num);
		a_table[i].value = start;
		a_table[i].stamp = 0;
		_copy[i] = (stamp_snap*)malloc(sizeof(stamp_snap) * thread_num);
		moved[i] = (bool*)malloc(sizeof(bool) * thread_num);
		result[i] = (long*)malloc(sizeof(long) * thread_num);
	}
}

stamp_snap * collect(long tid){
	for(int i=0; i<thread_num; i++)
		_copy[tid][i] = a_table[i];

	return _copy[tid];
}


/* Take a snapshot
 */
long * scan(long tid){
	int flag = 1;
	stamp_snap * old_copy;
	stamp_snap * new_copy;
	old_copy = collect(tid);
	while(1){
		new_copy = collect(tid);
		for(int i=0; i< thread_num; i++){
			if(old_copy[i].stamp != new_copy[i].stamp){
				if(moved[tid][i]){
					for(int j=0;j<thread_num; j++)
						moved[tid][j] = 0;
					return old_copy[i].snap;
				}
				else{
					moved[tid][i] = true;
					old_copy = new_copy;
					flag = 0;
					break;
				}
			}
			if(i==thread_num)
				flag = 1;
		}
		if(flag){
			for(int i=0;i<thread_num; i++)
				result[tid][i] = new_copy[i].value;
			return result[tid];
		}
	}
}


void update(long value, long tid){
	long * snap = scan(tid);
	stamp_snap old_value = a_table[tid];
	stamp_snap new_value[1];
	new_value[0].stamp = old_value.stamp + 1;
	new_value[0].value = value;
	new_value[0].snap = snap;
	a_table[tid] = new_value[0];
}

void* ThreadFunc(void * arg){
	long tid = (long)arg;
	clock_t start_t = clock();
	clock_t tmp;
	long count = 0;
	long value = tid;
	while(true){
		// while loop for 60 seconds
		tmp = clock() - start_t;
		if((tmp / CLOCKS_PER_SEC) > 59)
			return (void*)count;

		// Update 
		update(value++, tid);
		count++;
	}
	return NULL;
}

int main(int argc, char** argv){
	pthread_t * threads;
	long count_sum=0;
	long ret;

	// Initialize thread_num, a_table, threads 
	thread_num = atoi(argv[1]);
	threads = (pthread_t*)malloc(sizeof(pthread_t)*thread_num);
	wait_free_initialize(0);

	// Create threads 
	for(int i=0;i<thread_num; i++){
		if(pthread_create(&threads[i], 0, ThreadFunc, (void*)i) < 0){
			std::cout << "pthread_create error" << endl;
			return 0;
		}
	}	

	// Wait threads done
	for(int i=0;i<thread_num; i++){
		pthread_join(threads[i], (void**)&ret);
		count_sum += ret;
	}

	std::cout << "update: " << count_sum << endl;
	return 0;
}




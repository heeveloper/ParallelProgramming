#include "main.h"
using namespace std;

size_t N;	// the number of threads
size_t R;	// the number of records
size_t E;	// maximum of global execution order
int64_t* records;	// R records contain 64-bit integer
size_t global_exec;
struct lock_h* lock_table;
struct thread_stat* threads_stat;

pthread_mutex_t global_mtx = PTHREAD_MUTEX_INITIALIZER;

void* threadFunc(void* arg){
	size_t tid = (size_t)arg;
	size_t i, j, k;
	int64_t i_value, j_value, k_value;
	size_t commit_id=0;

	while(1){
		struct lock_h input1, input2, input3;
		randGenerator(&i, &j, &k, commit_id);
		
		// Create a lock object.
		input1.tid = tid;
		input1.type = READ_LOCK;
		input1.next = NULL;
		threads_stat[tid].ptrRecord[0] = &input1;
		
		// The moment that the lock is on the list of the record.
		// Enqueue
		pthread_mutex_lock(&global_mtx);

		lock_table[i].tail->next = &input1;
		input1.pre = lock_table[i].tail;
		lock_table[i].tail = &input1;
		
		// Try to catch the lock of the record
		if(readLockChecker(threads_stat[tid].ptrRecord[0])){
			// Read
			i_value = records[i];
			pthread_mutex_unlock(&global_mtx);	
		}
		else{
			if(isDeadLock(threads_stat[tid].ptrRecord[0], tid)){
				// Dequeue
				removeLock(i, threads_stat[tid].ptrRecord[0]);
				pthread_mutex_unlock(&global_mtx);
				continue;
			}
			else{
				pthread_mutex_unlock(&global_mtx);
				// Spin-lock
				while(!readLockChecker(threads_stat[tid].ptrRecord[0]) && threads_stat[tid].ptrRecord[0]->pre->pre != NULL){
					if(threads_stat[tid].ptrRecord[0]->pre == &lock_table[i]){
						break;
					}
					else if(readLockChecker(threads_stat[tid].ptrRecord[0])){
						break;
					}
				}
				i_value = records[i];
			}
		}	
		
		// Create a lock object
		input2.tid = tid;
		input2.type = WRITE_LOCK;
		input2.next = NULL;
		threads_stat[tid].ptrRecord[1] = &input2;

		// The moment that the lock is on the list of the record.
		// Enqueue
		pthread_mutex_lock(&global_mtx);
		lock_table[j].tail->next = &input2;
		input2.pre = lock_table[j].tail;
		lock_table[j].tail = &input2;
		
		// Try to catch the lock of the record
		if(threads_stat[tid].ptrRecord[1]->pre == &lock_table[j]){
			// Write
			j_value = records[j];
			records[j] += i_value + 1;
			pthread_mutex_unlock(&global_mtx);
		}
		else{
			if(isDeadLock(threads_stat[tid].ptrRecord[1], tid)){
				// Dequeue
				removeLock(i, threads_stat[tid].ptrRecord[0]);
				removeLock(j, threads_stat[tid].ptrRecord[1]);
				pthread_mutex_unlock(&global_mtx);
				continue;
			}
			else{
				pthread_mutex_unlock(&global_mtx);
				// Spin-lock
				while(threads_stat[tid].ptrRecord[1]->pre->pre != NULL){
				}
				j_value = records[j];
				records[j] += i_value + 1;
			}	
		}
		
		// Create a lock object
		input3.tid = tid;
		input3.type = WRITE_LOCK;
		input3.next = NULL;
		threads_stat[tid].ptrRecord[2] = &input3;
		
		// The moment that the lock is on the list of the record
		// Enqueue
		pthread_mutex_lock(&global_mtx);
		lock_table[k].tail->next = &input3;
		input3.pre = lock_table[k].tail;
		lock_table[k].tail = &input3;

		// Try to catch the lock of the record
		if(threads_stat[tid].ptrRecord[2]->pre == &lock_table[k]){
			// Write
			k_value = records[k];
			records[k] -= i_value;
			pthread_mutex_unlock(&global_mtx);
		}
		else{
			if(isDeadLock(threads_stat[tid].ptrRecord[2], tid)){
				// Dequeue
				removeLock(i, threads_stat[tid].ptrRecord[0]);
				removeLock(j, threads_stat[tid].ptrRecord[1]);
				removeLock(k, threads_stat[tid].ptrRecord[2]);
				pthread_mutex_unlock(&global_mtx);
				continue;
			}
			else{
				pthread_mutex_unlock(&global_mtx);
				// Spin-lock
				while(threads_stat[tid].ptrRecord[2]->pre != &lock_table[k]){
				}
				k_value = records[k];
				if(records[k] - i_value < INT64_MIN){
					exit(1);
				}
				records[k] -= i_value;
			}	
		}
/******************** Commit session**************************/

		// Acquire global mutex
		pthread_mutex_lock(&global_mtx);

		// Release all R/W lock
		// Dequeue
		removeLock(i, threads_stat[tid].ptrRecord[0]);
		removeLock(j, threads_stat[tid].ptrRecord[1]);
		removeLock(k, threads_stat[tid].ptrRecord[2]);
		// Get commit_id from the global execution order value
		commit_id = ++global_exec;
		if(commit_id > E){
			// Roll-back, then exit
			records[i] = i_value;
			records[j] = j_value;
			records[k] = k_value;
			pthread_mutex_unlock(&global_mtx);
			exit(1);
		}

		// Record the results
		printLog(tid, commit_id, i, j, k);
		pthread_mutex_unlock(&global_mtx);

	// End of while loop	
	}	

	return NULL;
}

int main(){
	global_exec = 0;
	pthread_t* threads;
	cin >> N >> R >> E;

	records = (int64_t*)malloc(sizeof(int64_t)*R);
	for(int i=0;i<R;i++){
		records[i] = 100;
	}

	// Dynamic allocatation for each thread
	threads = (pthread_t*)malloc(sizeof(pthread_t)*N);
	threads_stat = (struct thread_stat*)malloc(sizeof(struct thread_stat)*N);

	// Set arrays which means a lock table
	lock_table = (lock_h*)malloc(sizeof(lock_h)*R);
	for(int i=0;i<R;i++){
		lock_table[i].tail = &lock_table[i];
	}

	// Initialize threads_stat and create log files
	size_t count = 0;
	for(size_t i=0; i<N; i++){
		threads_stat[i].tid = count++;
		makeLog(i);
	}

	// Run threads
	for(size_t i=0;i<N;i++){
		if(pthread_create(&threads[i], 0, threadFunc,(void*)i) < 0){
			cout << "Error from pthread_create()" << endl;
			exit(1);
		}
	}
	
	while(1);	

	return 0;
}

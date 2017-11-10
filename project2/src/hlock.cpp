#include "hlock.h"

extern struct lock_h* lock_table;
extern struct thread_stat* threads_stat;

bool readLockChecker(struct lock_h* lock){
	lock_h* tmp = lock;
	while(tmp->pre != NULL){
		if(tmp->type == WRITE_LOCK){
			return false;
		}
		tmp = tmp->pre;
	}
	return true;
}

bool isDeadLock(struct lock_h* lock, size_t src){
	// Base case
	if(lock->pre == NULL){
		return false;
	}
	else if(lock->tid == src){
		return true;
	}

	// DFS search using Recursion
	while(lock->pre != NULL){
		if(isDeadLock(lock->pre, src)){
			return true;
		}
		lock = lock->pre;
	}
	return false;
}

void removeLock(int recordNum, struct lock_h* lock){
	if(lock->next == NULL){
		lock->pre->next = NULL;
		lock_table[recordNum].tail = lock->pre;
		lock = NULL;
	}
	else{
		lock->pre->next = lock->next;
		lock->next->pre = lock->pre;
		lock = NULL;
	}
	return;
}

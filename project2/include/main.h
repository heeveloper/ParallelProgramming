#ifndef MAIN_H
#define MAIN_H

#include <pthread.h>
#include "hlock.h"
#include "rand.h"
#include "logging.h"

// Random number generator
void randGenerator(size_t* i, size_t* j, size_t* k, size_t commit_id);

// Readlock checker
bool readLockChecker(struct lock_h* lock);

// Deadlock checker
bool isDeadlock(struct lock_h* lock, size_t src);

// Remove lock 
void removeLock(int recordNum, struct lock_h* lock);

// Create a log file(.txt)
void makeLog(size_t tid);

// Print the result on the .txt files
void printLog(size_t tid, size_t commit_id, size_t i, size_t j, size_t k);

// The function that each thread runs.
// Which also means the transaction.
void* threadFunc(void* arg);

#endif

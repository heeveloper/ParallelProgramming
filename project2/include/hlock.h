#ifndef HLOCK_H
#define HLOCK_H

#include <iostream>
#include "thread_stat.h"

bool readLockChecker(struct lock_h* lock);

bool isDeadLock(struct lock_h* lock, size_t src);

void removeLock(int recordNum, struct lock_h* lock);

#endif

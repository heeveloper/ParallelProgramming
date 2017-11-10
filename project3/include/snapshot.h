#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <pthread.h>
#include <stdlib.h>
#include <ctime>

typedef struct stamp_snap{
	long stamp;
	long value;
	long * snap;
} stamp_snap;

/* Snapshot functions definitions
 */
void wait_free_initialize(long init);
long * scan(long tid);
stamp_snap * collect(long tid);
void update(long value, long tid);

#endif

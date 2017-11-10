#include "lock_h.h"

struct thread_stat{
	// Thread number
	size_t tid;

	// Pointers which point to each record
	struct lock_h* ptrRecord[3];
};

// lock_h is a struct used to build a lock table

#define READ_LOCK 1
#define WRITE_LOCK 2

struct lock_h{
	// the owner of this lock
	size_t tid;
	
	// the type of lock
	// 0 for null, 1 for read_lock, 2 for write_lock
	int type;
	
	// pointers to next and pre
	struct lock_h* next;
	struct lock_h* pre;
	struct lock_h* tail;
};

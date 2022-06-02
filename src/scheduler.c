#include "../h/scheduler.h"
#include "../h/kernellib.h"
#include "../h/MemoryAllocator.h"
#include "../h/tcb.h"

struct __tcb* running = NULL;

struct __list* threads;

void __init_scheduler() {
	threads = __MA_allocate(sizeof(struct __list));
	threads->head = NULL;
	threads->tail = NULL;
	struct __tcb* kernel = __thread_create(NULL, NULL);
	//__scheduler_push(kernel);
	running = kernel;
}

void __scheduler_push(struct __tcb* newThread) {
	__push_back(threads, (void*)newThread);
}
struct __tcb* __scheduler_pop() {
	struct __tcb* front= __pop_front(threads);
	if(!front) return NULL;
	return (struct __tcb*)front;
}

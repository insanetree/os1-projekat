#include "../h/kernellib.h"
#include "../h/MemoryAllocator.h"
#include "../h/syscall_handlers.h"
#include "../h/scheduler.h"

void __handle_syscall();
void __interrupt();
void __interrupt_handler();

uint64 __align(uint64 what, uint64 to) {
	return what % to ? what + (to - what % to) : what;
}

inline void __init_system() {
	//memory initialization
	__MA_memory_init();

	__init_scheduler();

	//set interrupt_handler address in stvec
	__asm__ volatile("csrw stvec, %0 ": : "r" (&__interrupt));

	//enable interrupt ALWAYS AT THE END
	//__asm__ volatile("csrs sstatus, 0x02");
}

void __interrupt_handler() {
	uint64 scause;
	uint64 sepc;
	__asm__ volatile("csrr %0, sepc":"=r"(sepc));
	__asm__ volatile("csrr %0, scause":"=r"(scause));
	switch(scause) {
		case ((1ul << 63) | 1):
			//timer
			break;
		case 8:
		case 9:
			//ecall
			sepc+=4;
			__handle_syscall();
			__asm__ volatile("sd a0, 80(fp)");
			break;
	}
	__asm__ volatile("csrw sepc, %0"::"r"(sepc));
	__asm__ volatile("csrc sip, 0x02");
}

void __handle_syscall() {
	uint64 syscall;
	__asm__ volatile("mv %0, a0":"=r"(syscall));
	switch (syscall) {
		case 0x01:
			__mem_alloc_handler();
			break;
		case 0x02:
			__mem_free_handler();
			break;
		case 0x11:
			__thread_create_handler();
			break;
		case 0x12:
			//thread_exit
			break;
		case 0x13:
			__thread_dispatch();
			break;
	}
}

void __push_back(struct __list* list, void* elem) {
	struct __node* newNode = __MA_allocate(sizeof(struct __node));
	newNode->d = elem;
	newNode->next = NULL;
	if(!list->tail) {
		list->tail = newNode;
		list->head = newNode;
	} else {
		list->tail->next = newNode;
		list->tail = newNode;
	}
}

void* __pop_front(struct __list* list) {
	if(!list) return NULL;
	struct __node* front = list->head;
	if(!front) return NULL;
	list->head = list->head->next;
	if(list->head == NULL) list->tail = NULL;
	void* data = front->d;
	__MA_free(front);
	return data;
}


#ifndef OS1_RISC_V_PROJECT_SYSCALL_C_HPP
#define OS1_RISC_V_PROJECT_SYSCALL_C_HPP

#include "../h/tcb.hpp"
#include "../h/KernelSemaphore.hpp"

typedef TCB* thread_t;
typedef KernelSemaphore* sem_t;

// Memory management
void* mem_alloc(size_t size);
int mem_free(void* ptr);
size_t mem_get_free_space();
size_t mem_get_largest_free_block();

// Thread management
int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg);
int thread_exit();
void thread_dispatch();

// Semaphore management
int sem_open(sem_t* handle, uint64 init);
int sem_close(sem_t handle);
int sem_wait(sem_t id);
int sem_signal(sem_t id);

typedef unsigned long time_t;
int time_sleep(time_t);

// Console - not implemented, used from lib
const int EOF = -1;

char getc();
void putc(char);

#endif //OS1_RISC_V_PROJECT_SYSCALL_C_HPP
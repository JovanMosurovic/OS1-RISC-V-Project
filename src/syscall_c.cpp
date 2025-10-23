#include "../h/syscall_c.h"

#include "../h/syscall_asm.hpp"
#include "../h/syscall_codes.hpp"
#include "../lib/console.h"

using namespace SyscallAsm;

// Memory management

void *mem_alloc(size_t size) {
    return (void*)syscall1(SYS_MEM_ALLOC, size);
}

int mem_free(void *ptr) {
    return (int)syscall1(SYS_MEM_FREE, (uint64)ptr);
}

// Thread management

int thread_create(thread_t *handle, void (*start_routine)(void *), void *arg) {
    return (int)syscall3(SYS_THREAD_CREATE, (uint64)handle, (uint64)start_routine, (uint64)arg);
}

void thread_dispatch() {
    syscall0(SYS_THREAD_DISPATCH);
}

// Semaphore management

int sem_open(sem_t *handle, uint64 init) {
    return (int)syscall2(SYS_SEM_OPEN, (uint64)handle, (uint64)init);
}

int sem_close(sem_t handle) {
    return (int)syscall1(SYS_SEM_CLOSE, (uint64)handle);
}

int sem_wait(sem_t id) {
    return (int)syscall1(SYS_SEM_WAIT, (uint64)id);
}

int sem_signal(sem_t id) {
    return (int)syscall1(SYS_SEM_SIGNAL, (uint64)id);
}


// Time

int time_sleep(time_t) {
    return 0;
}

// Console - using lib functions

char getc() {
    return (char)syscall0(SYS_GETC);
}

void putc(char character) {
    syscall1(SYS_PUTC, (uint64)character);
}










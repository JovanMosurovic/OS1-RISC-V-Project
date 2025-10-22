#include "../h/syscall_c.hpp"

#include "../h/syscall_asm.hpp"
#include "../h/syscall_codes.hpp"

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
    return (int)syscall2(SYS_THREAD_CREATE, (uint64)handle, (uint64)start_routine());
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
    return (int)syscall1(SYS_SEM_CLOSE, (uint64)id);
}

int sem_signal(sem_t id) {
    return (int)syscall1(SYS_SEM_SIGNAL, (uint64)id);
}










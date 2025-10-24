#include "../h/syscall_cpp.hpp"

// Thread

Thread::Thread(void(*body)(void*), void* arg) : myHandle(nullptr), body(body), arg(arg) {}

Thread::Thread() : myHandle(nullptr), body(nullptr), arg(nullptr) {}

Thread::~Thread() {}

void Thread::threadWrapper(void *threadPtr) {
    Thread* thread = (Thread*)threadPtr;

    if (thread->body) {
        // Function-based thread
        thread->body(thread->arg);
    } else {
        // Virtual run() based thread
        thread->run();
    }

    thread_exit();
}

int Thread::start() {
    int result = thread_create(&myHandle, threadWrapper, (void*)this);
    return result;
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t) {
    // Not implemented
    return 0;
}

int Thread::getThreadId() {
    return (int)thread_get_id();
}

// Semaphore

Semaphore::Semaphore(unsigned init) : myHandle(nullptr) {
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    if (myHandle) {
        sem_close(myHandle);
    }
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

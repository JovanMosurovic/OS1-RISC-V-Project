#ifndef OS1_RISC_V_PROJECT_SYSCALL_CPP_HPP
#define OS1_RISC_V_PROJECT_SYSCALL_CPP_HPP

#include "syscall_c.hpp"

// Memory management

// ALREADY DONE IN operators.cpp
// void *operator new(size_t size);
// void operator delete(void *ptr) noexcept;
//
// void *operator new[](size_t size);
// void operator delete[](void *ptr) noexcept;

// Thread

class Thread {
public:
    Thread(void (*body)(void *), void *arg);
    virtual ~Thread();

    int start();

    static void dispatch();
    static int sleep(time_t);

protected:
    Thread();
    virtual void run() {}

private:
    thread_t myHandle;
    void (*body)(void *);
    void *arg;

    // Helper method
    static void threadWrapper(void* threadPtr);
};

// Semaphore

class Semaphore {
public:
    Semaphore(unsigned init = 1);
    virtual ~Semaphore();

    int wait();
    int signal();

private:
    sem_t myHandle;
};

#endif //OS1_RISC_V_PROJECT_SYSCALL_CPP_HPP


#ifndef OS1_RISC_V_PROJECT_SEMAPHORE_HPP
#define OS1_RISC_V_PROJECT_SEMAPHORE_HPP

#include "../lib/hw.h"
#include "list.hpp"
#include "tcb.hpp"


class KernelSemaphore {
public:
    KernelSemaphore(uint64 init = 1);
    ~KernelSemaphore();

    int wait();
    int signal();

    // get current value of the semaphore
    int getValue() const { return value; }

    // get number of threads waiting on this semaphore
    int getWaitingCount() const { return value < 0 ? -value : 0; }

private:
    int value;
    List<TCB> blockedQueue;

    KernelSemaphore(const KernelSemaphore&) = delete;
    KernelSemaphore& operator=(const KernelSemaphore&) = delete;
};

#endif //OS1_RISC_V_PROJECT_SEMAPHORE_HPP
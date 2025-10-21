
#ifndef OS1_RISC_V_PROJECT_SEMAPHORE_HPP
#define OS1_RISC_V_PROJECT_SEMAPHORE_HPP

#include "../lib/hw.h"
#include "list.hpp"
#include "tcb.hpp"


class Semaphore {
public:
    Semaphore(uint64 init = 1);
    ~Semaphore();

    int wait();
    int signal();

    // get current value of the semaphore
    int getValue() const { return value; }

    // get number of threads waiting on this semaphore
    int getWaitingCount() const { return value < 0 ? -value : 0; }

private:
    int value;
    List<TCB> blockedQueue;

    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
};

#endif //OS1_RISC_V_PROJECT_SEMAPHORE_HPP
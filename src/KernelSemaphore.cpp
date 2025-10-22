#include "../h/KernelSemaphore.hpp"

KernelSemaphore::KernelSemaphore(uint64 init) : value(init) {}

KernelSemaphore::~KernelSemaphore() {
    while (blockedQueue.peekFirst()) {
        TCB* tcb = blockedQueue.removeFirst();
        Scheduler::put(tcb);
    }
}

int KernelSemaphore::wait() {

    value--;

    if (value < 0) {
        blockedQueue.addLast(TCB::running);

        // Context switch without returning to scheduler
        TCB* old = TCB::running;
        TCB::running = Scheduler::get();

        if (!TCB::running) {
            // If no ready threads available -> ERROR
            TCB::running = old;
            blockedQueue.removeLast();
            value++;
            return -1;
        }

        TCB::contextSwitch(&old->context, &TCB::running->context);
    }

    return 0;
}

int KernelSemaphore::signal() {

    value++;

    if (value <= 0) {
        TCB *tcb = blockedQueue.removeFirst();

        if (tcb) {
            Scheduler::put(tcb);
        }
    }

    return 0;
}




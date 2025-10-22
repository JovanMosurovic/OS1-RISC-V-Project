#include "../h/semaphore.hpp"

Semaphore::Semaphore(uint64 init) : value(init) {}

Semaphore::~Semaphore() {
    while (blockedQueue.peekFirst()) {
        TCB* tcb = blockedQueue.removeFirst();
        Scheduler::put(tcb);
    }
}

int Semaphore::wait() {

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

int Semaphore::signal() {

    value++;

    if (value <= 0) {
        TCB *tcb = blockedQueue.removeFirst();

        if (tcb) {
            Scheduler::put(tcb);
        }
    }

    return 0;
}




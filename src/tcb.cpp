//
// Created by marko on 20.4.22..
//

#include "../h/tcb.hpp"
#include "../h/riscv.hpp"

TCB *TCB::running = nullptr;


// For C API -> already allocated stack
TCB *TCB::createThread(Body body, void *arg, void* stack_space) {
    if (!body || !stack_space) return nullptr;
    return new TCB(body, arg, stack_space, PRIVILEGE_USER);
}

// For already allocated stack
TCB *TCB::createThreadSimple(Body body, void *arg, ThreadPrivilege priv) {
    return new TCB(body, arg, priv);
}

TCB *TCB::createKernelThread(Body body, void* arg)
{
    return new TCB(body, arg, PRIVILEGE_SUPERVISOR);
}

void TCB::yield()
{
    if (running && running->getPrivilege() == PRIVILEGE_SUPERVISOR) {
        dispatch();
    }
    __asm__ volatile ("ecall");
}

int TCB::thread_exit() {
    if (!running) return -1;

    running->setFinished(true);
    yield();

    return 0;
}

void TCB::dispatch()
{
    TCB *old = running;
    if (!old->isFinished()) { Scheduler::put(old); }
    running = Scheduler::get();

    if (running) {
        TCB::contextSwitch(&old->context, &running->context);
    }
}

void TCB::threadWrapper()
{
    Riscv::popSppSpie();
    running->body(running->arg);
    running->setFinished(true);
    TCB::yield();
}

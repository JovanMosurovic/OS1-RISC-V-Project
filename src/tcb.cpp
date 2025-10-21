//
// Created by marko on 20.4.22..
//

#include "../h/tcb.hpp"
#include "../h/riscv.hpp"

TCB *TCB::running = nullptr;


TCB *TCB::createThread(Body body)
{
    return new TCB(body);
}

void TCB::yield()
{
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

    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper()
{
    Riscv::popSppSpie();
    running->body();
    running->setFinished(true);
    TCB::yield();
}

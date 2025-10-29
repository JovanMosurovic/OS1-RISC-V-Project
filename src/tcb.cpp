//
// Created by marko on 20.4.22..
//

#include "../h/tcb.hpp"
#include "../h/riscv.hpp"

uint64 TCB::maxThreads = 5;
uint64 TCB::activeThreads = 0;
List<TCB> TCB::blockedByMaxThreads;
bool TCB::calledMaxThreads = false;
TCB *TCB::running = nullptr;
uint64 TCB::nextThreadId = 0;

TCB * TCB::createThread(Body body, void *arg, void *stack) {
    return new TCB(body, arg, stack);
}

void TCB::yield()
{
    __asm__ volatile("li a0,0x13");
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
    if (old && !old->isFinished()) { Scheduler::put(old); }
    running = Scheduler::get();

    if (running && running != old) {
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

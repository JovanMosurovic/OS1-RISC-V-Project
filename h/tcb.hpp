//
// Created by marko on 20.4.22..
//

#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP

#include "../lib/hw.h"
#include "scheduler.hpp"

enum ThreadPrivilege {
    PRIVILEGE_USER = 0,         // User mode (U-mode)
    PRIVILEGE_SUPERVISOR = 1    // Supervisor mode (S-mode)
};

// Thread Control Block
class TCB
{
public:
    ~TCB() { delete[] stack; }

    bool isFinished() const { return finished; }
    void setFinished(bool value) { finished = value; }

    ThreadPrivilege getPrivilege() const { return privilege; }
    void setPrivilege(ThreadPrivilege priv) { privilege = priv; }

    uint64 getThreadId() const { return threadId; }

    using Body = void (*)(void*);

    static TCB* createThread(Body body, void* arg, void* stack);

    static void yield();
    static int thread_exit();

    static void dispatch();

    static TCB *running;

private:
    TCB(Body body, void* arg, void* stack) {
        this->body = body;
        this->arg = arg;
        this->stack = (uint64 *)stack;
        this->privilege = PRIVILEGE_USER;
        this->threadId = nextThreadId++;
        this->context = {
            (uint64) &threadWrapper,
            this->stack != nullptr ? (uint64) &this->stack[STACK_SIZE] : 0
        };
        this->finished = false;

        if (body != nullptr) { Scheduler::put(this); }
    }

    struct Context
    {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    void* arg;
    ThreadPrivilege privilege;
    uint64 *stack;
    Context context;
    bool finished;
    uint64 threadId;

    static uint64 nextThreadId;

    friend class Riscv;
    friend class KernelSemaphore;

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *runningContext);


    static uint64 constexpr STACK_SIZE = 1024;
};

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP

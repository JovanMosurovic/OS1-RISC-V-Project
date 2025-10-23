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

    using Body = void (*)(void*);

    static TCB* createThread(Body body, void* arg, void* stack_space);
    static TCB* createThreadSimple(Body body, void* arg = nullptr, ThreadPrivilege priv = PRIVILEGE_USER);
    static TCB* createKernelThread(Body body, void* arg = nullptr);

    static void yield();
    static int thread_exit();

    static void dispatch();

    static TCB *running;

private:
    // Constructor that receives an already allocated stack (for ABI call)
    TCB(Body body, void* arg, void* stack_space, ThreadPrivilege priv) :
            body(body),
            arg(arg),
            privilege(priv),
            stack((uint64*)stack_space),
            stackOwner(false),
            context({(uint64) &threadWrapper,(uint64)stack_space}),
            finished(false)
    {
        if (body != nullptr) { Scheduler::put(this); }
    }

    // Constructor that allocates the stack itself (for internal use)
    TCB(Body body, void* arg, ThreadPrivilege priv) :
            body(body),
            arg(arg),
            privilege(priv),
            stack(body != nullptr ? new uint64[STACK_SIZE] : nullptr),
            stackOwner(true),
            context({(uint64) &threadWrapper,
                     stack != nullptr ? (uint64) &stack[STACK_SIZE] : 0
                    }),
            finished(false)
    {
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
    bool stackOwner;
    Context context;
    bool finished;

    friend class Riscv;
    friend class KernelSemaphore;

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *runningContext);


    static uint64 constexpr STACK_SIZE = 1024;
};

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP

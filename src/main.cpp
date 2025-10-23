//
// Created by marko on 20.4.22..
//

#include "../h/MemoryAllocator.hpp"
#include "../h/tcb.hpp"
#include "../h/print.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.h"
#include "../lib/console.h"

extern void userMain();

TCB* kernel = nullptr;
TCB* user = nullptr;

int main()
{

    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);


    kernel = TCB::createKernelThread(nullptr, nullptr);
    TCB::running = kernel;

    user = TCB::createThread(reinterpret_cast<void (*)(void *)>(userMain), nullptr, mem_alloc(DEFAULT_STACK_SIZE));
    while(!user->isFinished()) {
        thread_dispatch();
    }

    return 0;
}
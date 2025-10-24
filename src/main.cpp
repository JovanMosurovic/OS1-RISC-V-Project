//
// Created by marko on 20.4.22..
//

#include "../h/MemoryAllocator.hpp"
#include "../h/tcb.hpp"
#include "../h/print.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.h"

extern void userMain();

TCB* kernel = nullptr;
TCB* user = nullptr;

void userMainWrapper(void*) {
    userMain();
}

int main()
{

    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);


    thread_create(&kernel, nullptr, nullptr);
    TCB::running = kernel;

    thread_create(&user, userMainWrapper, nullptr);

    while(!user->isFinished()) {
        TCB::dispatch();
    }

    return 0;
}
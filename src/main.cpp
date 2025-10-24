//
// Created by marko on 20.4.22..
//

#include "../h/MemoryAllocator.hpp"
#include "../h/tcb.hpp"
#include "../h/print.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.h"
#include "../lib/console.h"

// extern void userMain();

TCB* kernel = nullptr;
TCB* user = nullptr;

void test2() {
    __putc('b');
}

void test1(void*) {
    __putc('a');
    test2();
    __putc('c');
}

int main()
{

    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);

    auto* x = (uint64*)(mem_alloc(64));
    *x = 1;
    __putc('0' + *x);


    thread_create(&kernel, nullptr, nullptr);
    TCB::running = kernel;

    thread_create(&user, test1, nullptr);
    while(!user->isFinished()) {
        TCB::dispatch();
    }

    return 0;
}
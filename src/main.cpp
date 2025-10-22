//
// Created by marko on 20.4.22..
//

#include "../h/tcb.hpp"
#include "../h/workers.hpp"
#include "../h/print.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"

void cum(void* p) {
    uint64 x = (uint64)p;
    printInteger(x);
}

int main()
{
    TCB *threads[2];

    threads[0] = TCB::createKernelThread(nullptr);
    TCB::running = threads[0];

    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    int x = thread_create(&threads[1], cum, nullptr);
    printInteger(x);
    printString("ThreadA created\n");

    while (!threads[1]->isFinished())
    {
        TCB::yield();
    }

    for (auto &thread: threads)
    {
        delete thread;
    }
    printString("Finished\n");

    return 0;
}
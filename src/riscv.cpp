//
// Created by marko on 20.4.22..
//

#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../lib/console.h"

void handleSystemCall(uint64 code, uint64 arg1, uint64 arg2, uint64 arg3, uint64 arg4, uint64* retVal);

void Riscv::popSppSpie()
{
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

void Riscv::handleSupervisorTrap()
{
    uint64 scause = r_scause();
    if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL)
    {
        // interrupt: no; cause code: environment call from U-mode(8) or S-mode(9)
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();

        // System call arguments from registers
        uint64 code, arg1, arg2, arg3, arg4, retVal;

        __asm__ volatile ("mv %0, a0" : "=r"(code));
        __asm__ volatile ("mv %0, a1" : "=r"(arg1));
        __asm__ volatile ("mv %0, a2" : "=r"(arg2));
        __asm__ volatile ("mv %0, a3" : "=r"(arg3));
        __asm__ volatile ("mv %0, a4" : "=r"(arg4));

        handleSystemCall(code, arg1, arg2, arg3, arg4, &retVal);

        // Return value -> a0
        __asm__ volatile ("mv a0, %0" : : "r"(retVal));

        TCB::dispatch();
        w_sstatus(sstatus);
        w_sepc(sepc);
    }
    else if (scause == 0x8000000000000001UL)
    {
        // interrupt: yes; cause code: supervisor software interrupt (CLINT; machine timer interrupt)
        mc_sip(SIP_SSIP);
        uint64 volatile sepc = r_sepc();
        uint64 volatile sstatus = r_sstatus();
        TCB::dispatch();
        w_sstatus(sstatus);
        w_sepc(sepc);
    }
    else if (scause == 0x8000000000000009UL)
    {
        // interrupt: yes; cause code: supervisor external interrupt (PLIC; could be keyboard)
        console_handler();
    }
    else
    {
        // unexpected trap cause
    }
}
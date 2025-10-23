
#ifndef OS1_RISC_V_PROJECT_SYSCALL_ASM_HPP
#define OS1_RISC_V_PROJECT_SYSCALL_ASM_HPP

#include "../lib/hw.h"

namespace SyscallAsm {

    inline uint64 syscall0(uint64 code) {
        uint64 ret;

        __asm__ volatile (
            "mv a0, %[code]\n"
            "ecall\n"
            "mv %[ret], a0\n"
            : [ret] "=r"(ret)
            : [code] "r"(code)
            : "a0"
        );

        return ret;
    }

    inline uint64 syscall1(uint64 code, uint64 arg1) {
        uint64 ret;

        __asm__ volatile (
            "mv a0, %[code]\n"
            "mv a1, %[arg1]\n"
            "ecall\n"
            "mv %[ret], a0\n"
            : [ret] "=r"(ret)
            : [code] "r"(code),
              [arg1] "r"(arg1)
            : "a0", "a1"
        );

        return ret;
    }

    inline uint64 syscall2(uint64 code, uint64 arg1, uint64 arg2) {
        uint64 ret;

        __asm__ volatile (
            "mv a0, %[code]\n"
            "mv a1, %[arg1]\n"
            "mv a2, %[arg2]\n"
            "ecall\n"
            "mv %[ret], a0\n"
            : [ret] "=r"(ret)
            : [code] "r"(code),
              [arg1] "r"(arg1),
              [arg2] "r"(arg2)
            : "a0", "a1", "a2"
        );

        return ret;
    }

    inline uint64 syscall3(uint64 code, uint64 arg1, uint64 arg2, uint64 arg3) {
        uint64 ret;

        __asm__ volatile (
            "mv a0, %[code]\n"
            "mv a1, %[arg1]\n"
            "mv a2, %[arg2]\n"
            "mv a3, %[arg3]\n"
            "ecall\n"
            "mv %[ret], a0\n"
            : [ret] "=r"(ret)
            : [code] "r"(code),
              [arg1] "r"(arg1),
              [arg2] "r"(arg2),
              [arg3] "r"(arg3)
            : "a0", "a1", "a2", "a3"
        );

        return ret;
    }

    inline uint64 syscall4(uint64 code, uint64 arg1, uint64 arg2, uint64 arg3, uint64 arg4) {
        uint64 ret;
        __asm__ volatile (
            "mv a0, %[code]\n"
            "mv a1, %[arg1]\n"
            "mv a2, %[arg2]\n"
            "mv a3, %[arg3]\n"
            "mv a4, %[arg4]\n"
            "ecall\n"
            "mv %[ret], a0\n"
            : [ret] "=r"(ret)
            : [code] "r"(code),
              [arg1] "r"(arg1),
              [arg2] "r"(arg2),
              [arg3] "r"(arg3),
              [arg4] "r"(arg4)
            : "a0", "a1", "a2", "a3", "a4"
        );
        return ret;
    }

}

#endif //OS1_RISC_V_PROJECT_SYSCALL_ASM_HPP
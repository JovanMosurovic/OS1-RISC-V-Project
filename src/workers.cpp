//
// Created by marko on 20.4.22..
//

#include "../lib/hw.h"
#include "../h/tcb.hpp"
#include "../h/print.hpp"

void workerBodyA()
{
    for (uint64 i = 0; i < 10; i++)
    {
        printS("A: i=");
        printI(i);
        printS("\n");
        for (uint64 j = 0; j < 10000; j++)
        {
            for (uint64 k = 0; k < 30000; k++)
            {
                // busy wait
            }
            TCB::yield();
        }
    }
}

void workerBodyB()
{
    for (uint64 i = 0; i < 16; i++)
    {
        printS("B: i=");
        printI(i);
        printS("\n");
        for (uint64 j = 0; j < 10000; j++)
        {
            for (uint64 k = 0; k < 30000; k++)
            {
                // busy wait
            }
            TCB::yield();
        }
    }
}

static uint64 fibonacci(uint64 n)
{
    if (n == 0 || n == 1) { return n; }
    if (n % 10 == 0) { TCB::yield(); }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

void workerBodyC()
{
    uint8 i = 0;
    for (; i < 3; i++)
    {
        printS("C: i=");
        printI(i);
        printS("\n");
    }

    printS("C: yield\n");
    __asm__ ("li t1, 7");
    TCB::yield();

    uint64 t1 = 0;
    __asm__ ("mv %[t1], t1" : [t1] "=r"(t1));

    printS("C: t1=");
    printI(t1);
    printS("\n");

    uint64 result = fibonacci(12);
    printS("C: fibonaci=");
    printI(result);
    printS("\n");

    for (; i < 6; i++)
    {
        printS("C: i=");
        printI(i);
        printS("\n");
    }
    TCB::yield();
}

void workerBodyD()
{
    uint8 i = 10;
    for (; i < 13; i++)
    {
        printS("D: i=");
        printI(i);
        printS("\n");
    }

    printS("D: yield\n");
    __asm__ ("li t1, 5");
    TCB::yield();

    uint64 result = fibonacci(16);
    printS("D: fibonaci=");
    printI(result);
    printS("\n");

    for (; i < 16; i++)
    {
        printS("D: i=");
        printI(i);
        printS("\n");
    }
    TCB::yield();
}
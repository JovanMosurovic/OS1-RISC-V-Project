/*
SR:
Implementirati sistemski poziv int getThreadId() koji dohvata jedinstveni identifikator niti koja se trenutno izvršava.
getThreadId() takođe treba da obavi promenu konteksta (za ovaj deo pogledati modifikaciju iz jula 2022).

Potrebno je proširiti usluge klase Thread tako da zadovoljava sledeće uslove:
- Dodati statičku metodu SetMaximumThreads(int num_of_threads) koju korisnik poziva na početku programa.
  Ova metoda postavlja ograničenje na maksimalan broj korisničkih niti. Ukoliko korisnik ništa ne unese,
  podrazumevana vrednost je 5.
- Nakon postavljanja maksimalnog broja niti, moguće je kreirati dati broj niti, a nakon toga sve novokreirane niti se blokiraju.
  Kada jedna od kreiranih korisničkih niti završi svoje izvršavanje, ona odblokira jednu od prethodno blokiranih niti,
  i to u redosledu u kojem su blokirane.

EN:
Implement the system call int getThreadId() that retrieves the unique identifier of the currently running thread.
getThreadId() should also perform a context switch (for this part, refer to the modification from July 2022).

You need to extend the Thread class services to satisfy the following requirements:
- Add a static method SetMaximumThreads(int num_of_threads) which the user calls at the beginning of the program.
  This method sets a limit on the maximum number of user threads. If the user does not provide a value,
  the default is 5.
- After setting the maximum number of threads, it is possible to create up to that number of threads; after that,
  all newly created threads are blocked.
  When one of the previously created user threads finishes its execution, it unblocks one of the previously blocked threads,
  in the order in which they were blocked.
*/

#include "../h/MemoryAllocator.hpp"
#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.h"
#include "../h/syscall_cpp.hpp"
#include "../test/printing.hpp"

extern void userMain();

TCB* kernel = nullptr;
TCB* user = nullptr;

void userMainWrapper(void*) {
    userMain();
}

void worker(void*) {
    for (int i = 0; i < 5; ++i) {
        printString("Hello! Thread::");
        printInt(Thread::getThreadId());
        printString("\n");
    }
    while (1) {

    }
}

int main()
{

    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);

    set_maximum_threads(3);

    thread_create(&kernel, nullptr, nullptr);
    TCB::running = kernel;

    thread_t threads[20];
    for (int i = 0; i < 20; ++i) {
        thread_create(&threads[i], worker, nullptr);
    }

    // thread_create(&user, userMainWrapper, nullptr);

    // while(!user->isFinished()) {
    //     TCB::dispatch();
    // }

    bool allFinished = false;
    while (!allFinished) {
        allFinished = true;
        for (int i = 0; i < 20; ++i) {
            if (threads[i] && !threads[i]->isFinished()) {
                allFinished = false;
                break;
            }
        }
        if (!allFinished) {
            TCB::dispatch();
        }
    }

    return 0;
}
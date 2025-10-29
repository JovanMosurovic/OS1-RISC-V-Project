//
// Created by marko on 20.4.22..
//

#include "../h/MemoryAllocator.hpp"
#include "../h/tcb.hpp"
#include "../test/printing.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.h"
#include "../h/syscall_cpp.hpp"

extern void userMain();

const int NUM_THREADS = 20;

struct WorkerArgs {
    int id;
    void* data;
};

void workerC(void* arg) {
    auto myId = (uint64)thread_get_id();
    // WorkerArgs* args = (WorkerArgs*)arg;
    // int myId = args->id;

    for (int i = 0; i < 20; ++i) {
        printString("Hello! Thread::");
        printInt(myId);
        printString("\n");
    }

    for (volatile uint64 j = 0; j < myId * 1000; ++j) {
        // busy wait
    }

    // if (arg) delete (WorkerArgs*)arg;

    thread_exit();
}

class WorkerCPP : public Thread {
public:
    WorkerCPP () : Thread() {}

    ~WorkerCPP() override {}

protected:
    void run() override {
        int myId = Thread::getThreadId();

        for (int i = 0; i < 20; ++i) {
            printString("Hello! Thread::");
            printInt(myId);
            printString("\n");
        }

        for (volatile uint64 j = 0; j < (uint64)myId * 1000; ++j) {
            // busy wait
        }
    }
};

void userMainWrapper(void*) {
    userMain();
}

int main()
{
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);

    // ========== C API TEST ==========
    thread_t kernel = nullptr;
    thread_t user = nullptr;
    thread_create(&kernel, nullptr, nullptr);
    TCB::running = kernel;
    thread_create(&user, userMainWrapper, nullptr);

    // thread_t cThreads[NUM_THREADS];
    // for (int i = 0; i < NUM_THREADS; ++i) {
    //     thread_create(&cThreads[i], workerC, nullptr);
    // }

    // sem_t sem;
    // sem_open(&sem, 1);

    while (!user->isFinished()) {
        TCB::dispatch();
    }

    // THREADS WAITING
    // bool allDone = false;
    // while (!allDone) {
    //     allDone = true;
    //     for (int i = 0; i < NUM_THREADS; ++i) {
    //         if (cThreads[i] && !cThreads[i]->isFinished()) {
    //             allDone = false;
    //             break;
    //         }
    //     }
    //     if (!allDone) TCB::dispatch();
    // }

    // sem_close(sem);

    // // ========== C++ API TEST ==========
    // thread_t kernel = nullptr;
    // thread_create(&kernel, nullptr, nullptr);
    // TCB::running = kernel;
    //
    // auto* cppUser = new Thread(userMainWrapper, nullptr);
    // cppUser->start();
    //
    // // WorkerCPP* cppThreads[NUM_THREADS];
    // // for (int i = 0; i < NUM_THREADS; ++i) {
    // //     cppThreads[i] = new WorkerCPP();
    // //     cppThreads[i]->start();
    // // }
    //
    // // Semaphore* sem = new Semaphore(1);
    //
    // while (!cppUser->getHandle()->isFinished()) {
    //     Thread::dispatch();
    // }
    //
    // // THREADS WAITING
    // // bool allDone = false;
    // // while (!allDone) {
    // //     allDone = true;
    // //     for (int i = 0; i < NUM_THREADS; ++i) {
    // //         if (cppThreads[i]->getHandle() && !cppThreads[i]->getHandle()->isFinished()) {
    // //             allDone = false;
    // //             break;
    // //         }
    // //     }
    // //     if (!allDone) Thread::dispatch();
    // // }
    //
    // // delete sem;
    // // delete cppUser;
    // // for (int i = 0; i < NUM_THREADS; ++i) delete cppThreads[i];

    return 0;
}
#include "../h/MemoryAllocator.hpp"
#include "../h/tcb.hpp"
#include "../h/KernelSemaphore.hpp"
#include "../h/syscall_c.hpp"
#include "../h/syscall_codes.hpp"

void handleSystemCall(uint64 code, uint64 arg1, uint64 arg2, uint64* retVal) {
    switch (code) {

        // Memory management

        case SYS_MEM_ALLOC: {
            size_t size = (size_t)arg1;
            void* ptr = MemoryAllocator::getInstance().allocate(size);
            *retVal = (uint64)ptr;
            break;
        }

        case SYS_MEM_FREE: {
            void* ptr = (void*)arg1;
            int result = MemoryAllocator::getInstance().free(ptr);
            *retVal = result;
            break;
        }

        case SYS_MEM_GET_FREE_SPACE: {
            size_t freeSpace = MemoryAllocator::getInstance().getFreeSpace();
            *retVal = freeSpace;
            break;
        }

        case SYS_MEM_GET_LARGEST_FREE__BLOCK: {
            size_t largestBlock = MemoryAllocator::getInstance().getLargestFreeBlock();
            *retVal = largestBlock;
            break;
        }

        // Thread management

        case SYS_THREAD_CREATE: {
            thread_t* handle = (thread_t*)arg1;
            using Body = void (*)(void*);
            Body start_routine = (Body)arg2;

            if (!handle || !start_routine) {
                *retVal = -1;
                break;
            }

            TCB* thread = TCB::createThread(start_routine);

            if (!thread) {
                *retVal = -2;
                break;
            }

            *handle = thread;
            *retVal = 0;
            break;
        }

        case SYS_THREAD_EXIT: {
            *retVal = TCB::thread_exit();
            break;
        }

        case SYS_THREAD_DISPATCH: {
            // TCB::dispatch();
            *retVal = 0;
            break;
        }

        // Semaphore Management
        case SYS_SEM_OPEN: {
            sem_t* handle = (sem_t*)arg1;
            uint64 init = (uint64)arg2;

            if (!handle) {
                *retVal = -1;
                break;
            }

            KernelSemaphore* sem = new KernelSemaphore(init);

            if (!sem) {
                *retVal = -2;
                break;
            }

            *handle = sem;
            *retVal = 0;
            break;
        }

        case SYS_SEM_CLOSE: {
            sem_t handle = (sem_t)arg1;

            if (!handle) {
                *retVal = -1;
                break;
            }

            delete handle;
            *retVal = 0;
            break;
        }
        case SYS_SEM_WAIT: {
            sem_t id = (sem_t)arg1;

            if (!id) {
                *retVal = -1;
                break;
            }

            *retVal = id->wait();
            break;
        }

        case SYS_SEM_SIGNAL: {
            sem_t id = (sem_t)arg1;

            if (!id) {
                *retVal = -1;
                break;
            }

            *retVal = id->signal();
            break;
        }

        default:
            *retVal = 1;
            break;
    }
}
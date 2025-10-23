#include "../h/MemoryAllocator.hpp"

void* operator new(size_t size) {
    return MemoryAllocator::getInstance().allocate(size);
}

void operator delete(void* ptr) noexcept {
    MemoryAllocator::getInstance().free(ptr);
}

void* operator new[](size_t size) {
    return MemoryAllocator::getInstance().allocate(size);
}

void operator delete[](void* ptr) noexcept {
    MemoryAllocator::getInstance().free(ptr);
}




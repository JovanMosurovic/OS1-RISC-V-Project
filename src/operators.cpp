#include "../h/MemoryAllocator.hpp"

void* operator new(size_t size) {
    size_t sizeInBlocks = (size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    return MemoryAllocator::getInstance().allocate(sizeInBlocks);
}

void operator delete(void* ptr) noexcept {
    MemoryAllocator::getInstance().free(ptr);
}

void* operator new[](size_t size) {
    size_t sizeInBlocks = (size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    return MemoryAllocator::getInstance().allocate(sizeInBlocks);
}

void operator delete[](void* ptr) noexcept {
    MemoryAllocator::getInstance().free(ptr);
}




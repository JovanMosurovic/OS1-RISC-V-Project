#ifndef OS1_RISC_V_PROJECT_MEMORYALLOCATOR_H
#define OS1_RISC_V_PROJECT_MEMORYALLOCATOR_H

#include "../lib/hw.h"

struct BlockHeader {
    size_t dataSize;
    BlockHeader* next;
    bool isFree;
};

class MemoryAllocator {
public:
    static MemoryAllocator& getInstance();

    void* allocate(size_t sizeInBytes) const;
    int free(void* ptr) const;

    size_t getFreeSpace() const;
    size_t getLargestFreeBlock() const;

    MemoryAllocator(const MemoryAllocator&) = delete;
    MemoryAllocator& operator=(const MemoryAllocator&) = delete;

private:
    BlockHeader* freeListHead;
    bool initialized;

    static constexpr size_t HEADER_SIZE_BLOCKS = (sizeof(BlockHeader) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;

    MemoryAllocator();

    void init();

    void coalesce() const;
};

#endif //OS1_RISC_V_PROJECT_MEMORYALLOCATOR_H
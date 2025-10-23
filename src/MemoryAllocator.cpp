#include "../h/MemoryAllocator.hpp"

MemoryAllocator::MemoryAllocator() : freeListHead(nullptr), initialized(false) {
    init();
}

MemoryAllocator &MemoryAllocator::getInstance() {
    static MemoryAllocator instance;
    return instance;
}

void MemoryAllocator::init() {
    if (initialized) return;

    freeListHead = (BlockHeader*)HEAP_START_ADDR;

    const size_t totalBytes = (char*)HEAP_END_ADDR - (char*)HEAP_START_ADDR;
    const size_t totalBlocks = totalBytes / MEM_BLOCK_SIZE;

    freeListHead->dataSize = totalBlocks - HEADER_SIZE_BLOCKS;
    freeListHead->next = nullptr;
    freeListHead->isFree = true;

    initialized = true;
}

void MemoryAllocator::coalesce() const {
    BlockHeader *current = freeListHead;

    while (current && current->next) {
        if (current->isFree && current->next->isFree) {
            const char* currentEnd = (char*)current + (HEADER_SIZE_BLOCKS + current->dataSize) * MEM_BLOCK_SIZE;
            const char* nextStart = (char*)current->next;

            if (currentEnd == nextStart) {
                current->dataSize += HEADER_SIZE_BLOCKS + current->next->dataSize;
                current->next = current->next->next;
                continue;
            }
        }
        current = current->next;
    }
}

void *MemoryAllocator::allocate(const size_t sizeInBytes) const {
    if (sizeInBytes == 0) return nullptr;

    const size_t sizeInBlocks = (sizeInBytes + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;

    BlockHeader* bestFit = nullptr;
    BlockHeader *current = freeListHead;
    size_t minWaste = (size_t)-1;

    while (current) {
        if (current->isFree && current->dataSize >= sizeInBlocks) {
            const size_t waste = current->dataSize - sizeInBlocks;
            if (waste < minWaste) {
                minWaste = waste;
                bestFit = current;

                if (waste == 0) {
                    break;
                }
            }
        }
        current = current->next;
    }

    if (!bestFit) {
        return nullptr;
    }

    if (bestFit->dataSize > sizeInBlocks + HEADER_SIZE_BLOCKS) {
        const auto newBlock = (BlockHeader*)((char*)bestFit + (HEADER_SIZE_BLOCKS + sizeInBlocks) * MEM_BLOCK_SIZE);
        newBlock->dataSize = bestFit->dataSize - sizeInBlocks - HEADER_SIZE_BLOCKS;
        newBlock->next = bestFit->next;
        newBlock->isFree = true;

        bestFit->dataSize = sizeInBlocks;
        bestFit->next = newBlock;
    }

    bestFit->isFree = false;

    return (char*)bestFit + HEADER_SIZE_BLOCKS * MEM_BLOCK_SIZE;
}

int MemoryAllocator::free(void *ptr) const {
    if (!ptr) return 0;

    if (ptr < HEAP_START_ADDR || ptr >= HEAP_END_ADDR) {
        return -1;
    }

    const auto block = (BlockHeader*)((char*)ptr - HEADER_SIZE_BLOCKS * MEM_BLOCK_SIZE);

    if (block->isFree) {
        return -2;
    }

    block->isFree = true;

    coalesce();

    return 0;
}

size_t MemoryAllocator::getFreeSpace() const {
    size_t freeSpace = 0;
    BlockHeader* current = freeListHead;

    while (current) {
        if (current->isFree) {
            freeSpace += current->dataSize * MEM_BLOCK_SIZE;
        }
        current = current->next;
    }

    return freeSpace;
}

size_t MemoryAllocator::getLargestFreeBlock() const {
    size_t maxSize = 0;
    BlockHeader* current = freeListHead;

    while (current) {
        if (current->isFree && current->dataSize > maxSize) {
            maxSize = current->dataSize;
        }
        current = current->next;
    }

    return maxSize * MEM_BLOCK_SIZE;
}




#ifndef __MEMORYMANAGER_HPP
#define __MEMORYMANAGER_HPP

#include <Lib/Types.hpp>
#include <Multiboot.hpp>

#define BLOCK_SIZE 4096
#define BLOCKS_PER_BYTE 8

#define PAGE_SIZE 4096

#define phys2virt(x) ((u32) (x) + 3_GB)
#define virt2phys(x) ((u32) (x) - 3_GB)

class MemoryManager {
public:
    MemoryManager();

    void initialize(u32* grubMemoryMap, u32 grubMemoryMapSize);

    u32 getFreePhysicalPage(u32 size = 1);
    void freePhysicalPages(void* ptr, u32 size = 1);

private:
    u32 memorySize;

    u32* memoryMap;

    u32 usedBlocks;
    u32 maxiumumBlocks;

    void initializePMM(multiboot_mmap_entry* grubPhysMemoryMap, u32 grubMemoryMapSize);
//    void initializeVMM();

    u32 firstFreePhysicalPageBit(u32 size = 1);

    inline void setBit(u32 bit) {
        memoryMap[bit / 32] |= (1 << (bit % 32));
    }

    inline void unsetBit(u32 bit) {
        memoryMap[bit / 32] &= ~(1 << (bit % 32));
    }

    inline bool testBit(u32 bit) {
        return memoryMap[bit / 32] & (1 << (bit % 32));
    }

    void setRegionFree(u32 base, u32 size);
    void setRegionUsed(u32 base, u32 size);
};

#endif

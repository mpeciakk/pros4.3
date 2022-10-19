#ifndef __MEMORYMANAGER_HPP
#define __MEMORYMANAGER_HPP

#include <Lib/Types.hpp>
#include <Multiboot.hpp>

#define BLOCK_SIZE 4096
#define BLOCKS_PER_BYTE 8

#define PAGE_SIZE 4096

#define phys2virt(x) ((u32) (x) + 3_GB)
#define virt2phys(x) ((u32) (x) - 3_GB)

#define PD_INDEX(address) ((address) >> 22)
#define PT_INDEX(address) (((address) >> 12) & 0x3FF)
#define PAGE_PHYS_ADDRESS(dir_entry) ((*dir_entry) & ~0xFFF)

#define SET_ATTRIBUTE(entry, attr) (*entry |= attr)
#define CLEAR_ATTRIBUTE(entry, attr) (*entry &= ~attr)
#define TEST_ATTRIBUTE(entry, attr) (*entry & attr)
#define SET_FRAME(entry, address) (*entry = (*entry & ~0x7FFFF000) | (address & 0xFFFFF000)) // Only set address/frame, not flags

typedef enum {
    PTE_PRESENT = 0x01,
    PTE_READ_WRITE = 0x02,
    PTE_USER = 0x04,
    PTE_WRITE_THROUGH = 0x08,
    PTE_CACHE_DISABLE = 0x10,
    PTE_ACCESSED = 0x20,
    PTE_DIRTY = 0x40,
    PTE_PAT = 0x80,
    PTE_GLOBAL = 0x100,
    PTE_FRAME = 0x7FFFF000, // bits 12+
} PAGE_TABLE_FLAGS;

typedef enum {
    PDE_PRESENT = 0x01,
    PDE_READ_WRITE = 0x02,
    PDE_USER = 0x04,
    PDE_WRITE_THROUGH = 0x08,
    PDE_CACHE_DISABLE = 0x10,
    PDE_ACCESSED = 0x20,
    PDE_DIRTY = 0x40,       // 4MB entry only
    PDE_PAGE_SIZE = 0x80,   // 0 = 4KB page, 1 = 4MB page
    PDE_GLOBAL = 0x100,     // 4MB entry only
    PDE_PAT = 0x2000,       // 4MB entry only
    PDE_FRAME = 0x7FFFF000, // bits 12+
} PAGE_DIR_FLAGS;

typedef u32 PageTableEntry;
typedef u32 PageDirectoryEntry;

struct PageTable {
    PageTableEntry entries[1024];
} __attribute__((packed));

struct PageDirectory {
    PageDirectoryEntry entries[1024];
} __attribute__((packed));

class MemoryManager {
public:
    MemoryManager();

    static MemoryManager& instance() {
        static MemoryManager memoryManager;
        return memoryManager;
    }

    void initialize(u32* grubMemoryMap, u32 grubMemoryMapSize);

    u32 getFreePhysicalPage(u32 size = 1);
    void freePhysicalPages(void* ptr, u32 size = 1);

    void mapAddress(void* virtualAddress);
    void setPageDirectory(PageDirectory* pageDirectory);

private:
    u32 memorySize;

    // PMM
    u32* memoryMap;

    u32 usedBlocks;
    u32 maxiumumBlocks;

    void initializePMM(multiboot_mmap_entry* grubPhysMemoryMap, u32 grubMemoryMapSize);

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

    // VMM
    void initializeVMM();
    PageDirectory* currentPageDirectory;
};

#endif

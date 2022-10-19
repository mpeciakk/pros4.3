#include <Lib/Log.hpp>
#include <Lib/String.hpp>
#include <MM/MemoryManager.hpp>

extern "C" u32* kernelStart;
extern "C" u32* kernelEnd;

void MemoryManager::initializePMM(multiboot_mmap_entry* grubMemoryMap, u32 grubMemoryMapSize) {
    memoryMap = (u32*) (u32) &kernelEnd;
    klog(3, "PMM: Kernel memory map located at 0x%x", (u32) memoryMap);
    memset(memoryMap, 0xFF, usedBlocks / BLOCKS_PER_BYTE);

    maxiumumBlocks = memorySize / BLOCK_SIZE;
    usedBlocks = maxiumumBlocks;

    for (auto* entry = grubMemoryMap; (u32) entry < (u32) grubMemoryMap + grubMemoryMapSize;
         entry = (multiboot_mmap_entry*) ((u32) entry + entry->size + sizeof(entry->size))) {
        // TODO: add low and high together
        auto address = u32(entry->addr_low);
        auto length = entry->len_low;

        switch (entry->type) {
            case MULTIBOOT_MEMORY_AVAILABLE:
                klog(0, "PMM: Found memory region of type Available, address 0x%x and length %d", address, length);
                setRegionFree(address, length);
                break;
            case MULTIBOOT_MEMORY_RESERVED:
                klog(0, "PMM: Found memory region of type Reserved, address 0x%x and length %d", address, length);
                setRegionUsed(address, length);
                break;
            case (MULTIBOOT_MEMORY_ACPI_RECLAIMABLE):
                klog(0, "PMM: Found memory region of type ACPI_RECLAIMABLE, address 0x%x and length %d", address, length);
                setRegionUsed(address, length);
                break;
            case (MULTIBOOT_MEMORY_NVS):
                klog(0, "PMM: Found memory region of type NVS, address 0x%x and length %d", address, length);
                setRegionUsed(address, length);
                break;
            case (MULTIBOOT_MEMORY_BADRAM):
                klog(1, "PMM: Detected bad memory range, address 0x%x and length %d");
                setRegionUsed(address, length);
                break;
            default:
                klog(1, "PMM: Unknown range, address 0x%x and length %d");
                setRegionUsed(address, length);
                break;
        }
    }

    setRegionUsed(0x00000000, 1_MB);
    setRegionUsed(virt2phys((u32) &kernelStart), (u32) &kernelEnd - (u32) &kernelStart);
    setRegionUsed(virt2phys((u32) &kernelEnd), memorySize / 8 / 4096);

    klog(3, "PMM: maximumBlocks: %d, usedBlocks: %d, freeBlocks: %d", maxiumumBlocks, usedBlocks, maxiumumBlocks - usedBlocks);
}

void MemoryManager::setRegionFree(u32 base, u32 size) {
    for (u32 pageBase = base; pageBase <= (base + size); pageBase += PAGE_SIZE) {
        unsetBit(pageBase / PAGE_SIZE);
        usedBlocks--;
    }
}

void MemoryManager::setRegionUsed(u32 base, u32 size) {
    for (u32 pageBase = base; pageBase <= (base + size); pageBase += PAGE_SIZE) {
        setBit(pageBase / PAGE_SIZE);
        usedBlocks++;
    }
}

u32 MemoryManager::firstFreePhysicalPageBit(u32 size) {
    if (size == 0) {
        return -1;
    }

    for (u32 i = 0; i < memorySize / 32 / 4096; i++) {
        if (memoryMap[i] != 0xFFFFFFFF) {
            for (int j = 0; j < 32; j++) {
                if (!(memoryMap[i] & (1 << j))) {
                    int currentBit = i * 32 + j;

                    u32 free = 0;
                    for (u32 k = 0; k < size; k++) {
                        if (!testBit(currentBit + k)) {
                            free++;
                        }

                        if (free == size) {
                            return currentBit;
                        }
                    }
                }
            }
        }
    }

    return -1;
}

u32 MemoryManager::getFreePhysicalPage(u32 size) {
    if (maxiumumBlocks - usedBlocks <= size) {
        klog(1, "PMM: Run out of blocks");
        return 0;
    }

    u32 frame = firstFreePhysicalPageBit(size);

    if (frame == (u32) -1) {
        klog(1, "PMM: Not enough space");
        return 0;
    }

    for (u32 i = 0; i < size; i++) {
        setBit(frame + i);
    }

    u32 address = frame * BLOCK_SIZE;
    usedBlocks += size;

    klog(3, "PMM: Allocating blocks %d-%d", frame, frame + size);

    return address;
}

void MemoryManager::freePhysicalPages(void* ptr, u32 size) {
    u32 address = (u32) ptr;
    u32 frame = address / BLOCK_SIZE;

    klog(3, "PMM: Deallocating blocks %d-%d", frame, frame + size);

    for (u32 i = 0; i < size; i++) {
        unsetBit(frame + i);
    }

    usedBlocks--;
}

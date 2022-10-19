#include <Lib/String.hpp>
#include <Lib/Log.hpp>
#include <MM/MemoryManager.hpp>

void MemoryManager::initializeVMM() {
    auto* dir = (PageDirectory*) phys2virt(getFreePhysicalPage(3));
    memset(dir, 0, sizeof(PageDirectory));

    auto* table = (PageTable*) phys2virt(getFreePhysicalPage());
    memset(table, 0, sizeof(PageTable));

    klog(3, "VMM: Kernel page directory at 0x%x, kernel page table at 0x%x", (u32) dir, (u32) table);

    for (u32 i = 0, physicalAddress = 0, virtualAddress = 0xC0000000; i < 1024; i++, physicalAddress += PAGE_SIZE, virtualAddress += PAGE_SIZE) {
        PageTableEntry page = 0;
        SET_ATTRIBUTE(&page, PTE_PRESENT);
        SET_ATTRIBUTE(&page, PTE_READ_WRITE);
        SET_FRAME(&page, physicalAddress);

        table->entries[PT_INDEX(virtualAddress)] = page;
    }

    PageDirectoryEntry* entry = &dir->entries[PD_INDEX(0xC0000000)];
    SET_ATTRIBUTE(entry, PDE_PRESENT);
    SET_ATTRIBUTE(entry, PDE_READ_WRITE);
    SET_FRAME(entry, (u32) virt2phys(table));

    setPageDirectory(dir);

   for (u32 i = 0xC0500000; i < 0xC0500000 + 0x100000; i += PAGE_SIZE) {
       mapAddress((void*) i);
   }
}

void MemoryManager::mapAddress(void* virtualAddress) {
    auto* pd = currentPageDirectory;

    // Get page directory entry for virtual address
    auto* entry = &pd->entries[PD_INDEX((u32) virtualAddress)];

    if (!TEST_ATTRIBUTE(entry, PTE_PRESENT)) {
        // Page table is not present so create it
        auto* table = (PageTable*) phys2virt(getFreePhysicalPage());
        memset(table, 0, sizeof(PageTable));
        auto* entry = &pd->entries[PD_INDEX((u32) virtualAddress)];

        SET_ATTRIBUTE(entry, PDE_PRESENT);
        SET_ATTRIBUTE(entry, PDE_READ_WRITE);
        SET_FRAME(entry, (u32) virt2phys(table));
    }

    auto* table = (PageTable*) PAGE_PHYS_ADDRESS(entry);

    auto* page = (PageTableEntry*) phys2virt(&table->entries[PT_INDEX((u32) virtualAddress)]);

    auto physicalAddress = getFreePhysicalPage();

    SET_ATTRIBUTE(page, PTE_PRESENT);
    SET_ATTRIBUTE(page, PTE_READ_WRITE);
    SET_FRAME(page, physicalAddress);
}

void MemoryManager::setPageDirectory(PageDirectory* pageDirectory) {
    currentPageDirectory = pageDirectory;
    asm("movl %0, %%cr3" : : "r"(virt2phys(pageDirectory)));
    asm("invlpg 0");
}
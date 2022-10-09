#include <VGA.hpp>
#include <Lib/Log.hpp>
#include <Hardware/GDT.hpp>
#include <Hardware/IDT.hpp>
#include <Debug/Tests.hpp>
#include <MM/MemoryManager.hpp>
#include <Multiboot.hpp>

extern "C" [[noreturn]] void kmain(multiboot_info* physMbi, unsigned int multibootMagic) {
    auto* mbi = (multiboot_info*) phys2virt(physMbi);

    VGA::instance().clear();
    log(0, "Kernel loaded!");

    GDT gdt;

    InterruptManager::instance();

    MemoryManager memoryManager;
    memoryManager.initialize((u32*) mbi->mmap_addr, mbi->mmap_length);

    void* a = (void*) phys2virt(memoryManager.getFreePhysicalPage());

    memcpy(a, (void*) "testowansko2137\0", 16);

    log(3, "%s", (char*) a);

    memoryManager.freePhysicalPages((void*) virt2phys(a));

    performTests();

    while (true) {}
}
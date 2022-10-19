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
    klog(3, "KMAIN: GDT initialized");

    InterruptManager::instance();
    klog(3, "KMAIN: Interrupts enabled");

    MemoryManager::instance();
    MemoryManager::instance().initialize((u32*) mbi->mmap_addr, mbi->mmap_length);
    klog(3, "KMAIN: Memory manager enabled");

    // performTests();

    while (true) {}
}
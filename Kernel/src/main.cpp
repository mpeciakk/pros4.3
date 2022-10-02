#include <VGA.hpp>
#include <Lib/Log.hpp>
#include <Hardware/GDT.hpp>
#include <Hardware/IDT.hpp>

extern "C" [[noreturn]] void kmain() {
    VGA::instance().clear();
    log(0, "Kernel loaded!");

    GDT gdt;

    InterruptManager idt;

    while (true) {}
}
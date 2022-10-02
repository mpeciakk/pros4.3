#include <Hardware/IDT.hpp>
#include <Lib/Log.hpp>
#include <Hardware/Exceptions.hpp>

#define IDT_INTERRUPT_GATE 0x8E
#define IDT_TRAP_GATE 0x8F

#define HARDWARE_INTERRUPT_OFFSET 0x20

extern "C" void ignoreInterrupt();

InterruptManager::InterruptManager() : masterCommandPort(0x20), masterDataPort(0x21), slaveCommandPort(0xA0), slaveDataPort(0xA1) {
    u16 codeSegment = 0x08;

    DivideByZeroException divideByZeroException;
    DoubleFaultException doubleFaultException;
    InvalidTSSException invalidTSSException;
    GeneralProtectionFaultException generalProtectionFaultException;
    PageFaultException pageFaultException;

    handlers[0x00] = &divideByZeroException;
    handlers[0x08] = &doubleFaultException;
    handlers[0x0A] = &invalidTSSException;
    handlers[0x0D] = &generalProtectionFaultException;
    handlers[0x0E] = &pageFaultException;

    for (u8 i = 0; i < 255; i++) {
        setIDTEntry(i, &ignoreInterrupt, codeSegment, IDT_INTERRUPT_GATE);
    }

    setIDTEntry(0x00, &handleException0x00, codeSegment, IDT_TRAP_GATE);
    setIDTEntry(0x01, &handleException0x01, codeSegment, IDT_TRAP_GATE);
    setIDTEntry(0x02, &handleException0x02, codeSegment, IDT_TRAP_GATE);
    setIDTEntry(0x03, &handleException0x03, codeSegment, IDT_TRAP_GATE);
    setIDTEntry(0x04, &handleException0x04, codeSegment, IDT_TRAP_GATE);
    setIDTEntry(0x05, &handleException0x05, codeSegment, IDT_TRAP_GATE);
    setIDTEntry(0x06, &handleException0x06, codeSegment, IDT_TRAP_GATE);
    setIDTEntry(0x07, &handleException0x07, codeSegment, IDT_TRAP_GATE);
    setIDTEntry(0x08, &handleException0x08, codeSegment, IDT_TRAP_GATE);
    setIDTEntry(0x09, &handleException0x09, codeSegment, IDT_TRAP_GATE);
    setIDTEntry(0x0A, &handleException0x0A, codeSegment, IDT_TRAP_GATE);
    setIDTEntry(0x0B, &handleException0x0B, codeSegment, IDT_TRAP_GATE);
    setIDTEntry(0x0C, &handleException0x0C, codeSegment, IDT_TRAP_GATE);
    setIDTEntry(0x0D, &handleException0x0D, codeSegment, IDT_TRAP_GATE);
    setIDTEntry(0x0E, &handleException0x0E, codeSegment, IDT_TRAP_GATE);
    setIDTEntry(0x0F, &handleException0x0F, codeSegment, IDT_TRAP_GATE);
    setIDTEntry(0x10, &handleException0x10, codeSegment, IDT_TRAP_GATE);
    setIDTEntry(0x11, &handleException0x11, codeSegment, IDT_TRAP_GATE);
    setIDTEntry(0x12, &handleException0x12, codeSegment, IDT_TRAP_GATE);
    setIDTEntry(0x13, &handleException0x13, codeSegment, IDT_TRAP_GATE);

    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x00, &handleInterrupt0x00, codeSegment, IDT_INTERRUPT_GATE);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x01, &handleInterrupt0x01, codeSegment, IDT_INTERRUPT_GATE);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x02, &handleInterrupt0x02, codeSegment, IDT_INTERRUPT_GATE);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x03, &handleInterrupt0x03, codeSegment, IDT_INTERRUPT_GATE);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x04, &handleInterrupt0x04, codeSegment, IDT_INTERRUPT_GATE);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x05, &handleInterrupt0x05, codeSegment, IDT_INTERRUPT_GATE);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x06, &handleInterrupt0x06, codeSegment, IDT_INTERRUPT_GATE);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x07, &handleInterrupt0x07, codeSegment, IDT_INTERRUPT_GATE);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x08, &handleInterrupt0x08, codeSegment, IDT_INTERRUPT_GATE);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x09, &handleInterrupt0x09, codeSegment, IDT_INTERRUPT_GATE);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x0A, &handleInterrupt0x0A, codeSegment, IDT_INTERRUPT_GATE);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x0B, &handleInterrupt0x0B, codeSegment, IDT_INTERRUPT_GATE);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x0C, &handleInterrupt0x0C, codeSegment, IDT_INTERRUPT_GATE);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x0D, &handleInterrupt0x0D, codeSegment, IDT_INTERRUPT_GATE);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x0E, &handleInterrupt0x0E, codeSegment, IDT_INTERRUPT_GATE);
    setIDTEntry(HARDWARE_INTERRUPT_OFFSET + 0x0F, &handleInterrupt0x0F, codeSegment, IDT_INTERRUPT_GATE);
    setIDTEntry(0x80, &handleInterrupt0x80, codeSegment, 0x60);

    masterCommandPort.write(0x11);
    slaveCommandPort.write(0x11);

    masterDataPort.write(HARDWARE_INTERRUPT_OFFSET);
    slaveDataPort.write(HARDWARE_INTERRUPT_OFFSET + 8);

    masterDataPort.write(0x04);
    slaveDataPort.write(0x02);

    masterDataPort.write(0x01);
    slaveDataPort.write(0x01);

    masterDataPort.write(0x00);
    slaveDataPort.write(0x00);

    IDTPointer pointer;
    pointer.size = 256 * sizeof(IDTEntry) - 1;
    pointer.base = (u32) idt;
    asm volatile("lidt %0" : : "m"(pointer));

    asm volatile("sti");
}

void InterruptManager::setIDTEntry(u8 interrupt, void (*handler)(), u16 codeSegment, u8 type) {
    u32 address = (u32) handler;

    idt[interrupt].lowBits = address & 0xFFFF;
    idt[interrupt].highBits = (address >> 16) & 0xFFFF;
    idt[interrupt].codeSegment = codeSegment;
    idt[interrupt].type = type;
    idt[interrupt].zero = 0;
}

u32 InterruptManager::doHandleInterrupt(u8 interrupt, u32 esp) {
    if (handlers[interrupt] != nullptr) {
        esp = handlers[interrupt]->handle(esp);
    }

    if (HARDWARE_INTERRUPT_OFFSET <= interrupt && interrupt < HARDWARE_INTERRUPT_OFFSET + 16) {
        masterCommandPort.write(0x20);

        if (HARDWARE_INTERRUPT_OFFSET + 8 <= interrupt) {
            slaveCommandPort.write(0x20);
        }
    }

    return esp;
}

u32 InterruptManager::handleInterrupt(u8 interrupt, u32 esp) {
    return InterruptManager::instance().doHandleInterrupt(interrupt, esp);
}

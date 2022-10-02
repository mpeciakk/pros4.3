#ifndef __IDT_HPP
#define __IDT_HPP

#include <Lib/Types.hpp>
#include <Hardware/Port.hpp>

struct IDTEntry {
    u16 lowBits;
    u16 codeSegment;
    u8 zero;
    u8 type;
    u16 highBits;
} __attribute__((packed));

struct IDTPointer {
    u16 size;
    u32 base;
} __attribute__((packed));


extern "C" void handleException0x00();
extern "C" void handleException0x01();
extern "C" void handleException0x02();
extern "C" void handleException0x03();
extern "C" void handleException0x04();
extern "C" void handleException0x05();
extern "C" void handleException0x06();
extern "C" void handleException0x07();
extern "C" void handleException0x08();
extern "C" void handleException0x09();
extern "C" void handleException0x0A();
extern "C" void handleException0x0B();
extern "C" void handleException0x0C();
extern "C" void handleException0x0D();
extern "C" void handleException0x0E();
extern "C" void handleException0x0F();
extern "C" void handleException0x10();
extern "C" void handleException0x11();
extern "C" void handleException0x12();
extern "C" void handleException0x13();

extern "C" void handleInterrupt0x00();
extern "C" void handleInterrupt0x01();
extern "C" void handleInterrupt0x02();
extern "C" void handleInterrupt0x03();
extern "C" void handleInterrupt0x04();
extern "C" void handleInterrupt0x05();
extern "C" void handleInterrupt0x06();
extern "C" void handleInterrupt0x07();
extern "C" void handleInterrupt0x08();
extern "C" void handleInterrupt0x09();
extern "C" void handleInterrupt0x0A();
extern "C" void handleInterrupt0x0B();
extern "C" void handleInterrupt0x0C();
extern "C" void handleInterrupt0x0D();
extern "C" void handleInterrupt0x0E();
extern "C" void handleInterrupt0x0F();
extern "C" void handleInterrupt0x80();

class InterruptManager {
public:
    InterruptManager();

    static InterruptManager& instance() {
        static InterruptManager interruptManager;
        return interruptManager;
    }

private:
    __attribute__((aligned(0x10))) IDTEntry idt[256];

    Port8BitSlow masterCommandPort;
    Port8BitSlow masterDataPort;
    Port8BitSlow slaveCommandPort;
    Port8BitSlow slaveDataPort;

    void setIDTEntry(u8 interrupt, void (*handler)(), u16 codeSegment, u8 type);
    u32 doHandleInterrupt(u8 interrupt, u32 esp);

    static u32 handleInterrupt(u8 interrupt, u32 esp);
};

#endif

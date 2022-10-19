#include <Hardware/GDT.hpp>
#include <Lib/String.hpp>
#include <Lib/Log.hpp>

#define MAX_DESCRIPTORS 5

GDT::GDT() {
    setDescriptor(0, 0, 0, 0, 0);
    setDescriptor(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    setDescriptor(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    setDescriptor(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    setDescriptor(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    GDTPointer pointer{};
    pointer.size = (MAX_DESCRIPTORS * sizeof(Descriptor)) - 1;
    pointer.base = (u32) &descriptors[0];
    asm volatile("lgdt %0" : : "m"(pointer));

    asm("mov %0, %%ax;"
        "mov %%ax, %%ds;"
        "mov %%ax, %%es;"
        "mov %%ax, %%fs;"
        "mov %%ax, %%gs;"
        "ljmp %1, $next;"
        "next:;"
        "mov %0, %%ax;"
        "mov %%ax, %%ss;"
        :
        : "i"(0x10), "i"(0x08));
}

void GDT::setDescriptor(u32 i, u32 base, u32 limit, u8 access, u8 grand) {
    memset((void*) &descriptors[i], 0, sizeof(Descriptor));

    klog(3, "GDT: Descriptor %d, base: 0x%x, limit: 0x%x, access: 0x%x, grand: 0x%x", i, base, limit, access, grand);

    descriptors[i].baseLow = base & 0xFFFF;
    descriptors[i].baseMid = (base >> 16) & 0xFF;
    descriptors[i].baseHigh = (base >> 24) & 0xFF;
    descriptors[i].limit = limit & 0xFFFF;

    descriptors[i].flags = access;
    descriptors[i].grand = (limit >> 16) & 0x0F;
    descriptors[i].grand |= grand & 0xF0;
}
#ifndef __EXCEPTIONS_HPP
#define __EXCEPTIONS_HPP

#include <Hardware/IDT.hpp>
#include <Lib/Log.hpp>

class DivideByZeroException : public InterruptHandler {
    u32 handle(u32 esp) override {
        panic("Division by zero detected!");

        return esp;
    }
};

class DoubleFaultException : public InterruptHandler {
    u32 handle(u32 esp) override {
        panic("Double fault detected!");

        return esp;
    }
};

class InvalidTSSException : public InterruptHandler {
    u32 handle(u32 esp) override {
        panic("Invalid TSS exception detected!");

        return esp;
    }
};

class GeneralProtectionFaultException : public InterruptHandler {
    u32 handle(u32 esp) override {
        panic("General protection fault detected!");

        return esp;
    }
};

class PageFaultException : public InterruptHandler {
    u32 handle(u32 esp) override {
        panic("Page fault detected!");

        return esp;
    }
};

#endif
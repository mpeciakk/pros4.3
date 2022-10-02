.set IRQ_BASE, 0x20

.section .data
interruptnumber: .byte 0

.section .text

.macro handleException num
.global handleException\num
handleException\num:
    movb $\num, (interruptnumber)
    jmp int_bottom
.endm


.macro handleInterrupt num
.global handleInterrupt\num
handleInterrupt\num:
    pushl $0
    movb $\num + IRQ_BASE, (interruptnumber)
    jmp int_bottom
.endm

handleException 0x00
handleException 0x01
handleException 0x02
handleException 0x03
handleException 0x04
handleException 0x05
handleException 0x06
handleException 0x07
handleException 0x08
handleException 0x09
handleException 0x0A
handleException 0x0B
handleException 0x0C
handleException 0x0D
handleException 0x0E
handleException 0x0F
handleException 0x10
handleException 0x11
handleException 0x12
handleException 0x13

handleInterrupt 0x00
handleInterrupt 0x01
handleInterrupt 0x02
handleInterrupt 0x03
handleInterrupt 0x04
handleInterrupt 0x05
handleInterrupt 0x06
handleInterrupt 0x07
handleInterrupt 0x08
handleInterrupt 0x09
handleInterrupt 0x0A
handleInterrupt 0x0B
handleInterrupt 0x0C
handleInterrupt 0x0D
handleInterrupt 0x0E
handleInterrupt 0x0F
handleInterrupt 0x80

int_bottom:
    cli

    pushl %esp
    push (interruptnumber)
    call _ZN16InterruptManager15handleInterruptEhj
    mov %eax, %esp

    add $4, %esp

    sti
    iret

.global ignoreInterrupt
.type ignoreInterrupt, @function
ignoreInterrupt:
    iret

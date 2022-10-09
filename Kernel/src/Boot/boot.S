.set ALIGN,      1 << 0
.set MEMINFO,    1 << 1             # provide memory map by grub
.set FLAGS,      ALIGN | MEMINFO
.set MAGIC,      0x1BADB002
.set CHECKSUM,   -(MAGIC + FLAGS)

# Multiboot header that marks the program as kernel
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.set KERNEL_VIRTUAL_BASE, 0xC0000000
.set KERNEL_PAGE_NUMBER, (KERNEL_VIRTUAL_BASE >> 22)

.section .bss, "aw", @nobits
.align 16
stack_bottom:
.skip 16384 # 16KB
stack_top:

.section .data
.align 0x1000
.global bootPageDirectory
bootPageDirectory:
    # identity map first page (4MB)
    .long 0x00000083

    # zero pages before kernel
    .rept (KERNEL_PAGE_NUMBER - 1)
    .long 0
    .endr

    # identity map kernel page (4MB)
    .long 0x00000083

    # pages after kernel
    .rept (1024 - KERNEL_PAGE_NUMBER - 1)
    .long 0
    .endr

.section .text
.global _start
.type _start, @function
_start:
    # load our boot page directory
    mov $(bootPageDirectory - KERNEL_VIRTUAL_BASE), %ecx
    mov %ecx, %cr3

    # enable 4 mb pages
    mov %cr4, %ecx
    or $0x00000010, %ecx
    mov %ecx, %cr4

    # enable paging
    mov %cr0, %ecx
    or $0x80000001, %ecx
    mov %ecx, %cr0

    # jump to higher half code
    lea 4f, %ecx
    jmp *%ecx

4:
    movl $0, bootPageDirectory
    invlpg 0

    mov $stack_top, %esp

    push %ebx
    call kmain

    cli
1:
    hlt
    jmp 1b


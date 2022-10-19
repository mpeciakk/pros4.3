ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)

SECTIONS {
    . = 0xC0100000; /* Starts at 3GB + 1MB. */

    kernelStart = .;

	.text ALIGN(4K) : AT(ADDR(.text) - 0xC0000000) {
		*(.multiboot)
		*(.text*)
	}

	.rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xC0000000) {
		*(.rodata*)
	}

	.bss ALIGN(4K) : AT(ADDR(.bss) - 0xC0000000) {
		*(COMMON)
		*(.bss*)
	}

	.data ALIGN(4K) : AT(ADDR(.data) - 0xC0000000) {
		*(.data)
	}

	.other : {
        *(.debug*)
        *(.comment*)
    }

	kernelEnd = .;
}
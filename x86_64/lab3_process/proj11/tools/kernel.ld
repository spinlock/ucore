/* Simple linker script for the ucore kernel.
   See the GNU ld 'info' manual ("info ld") to learn the syntax. */

OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64", "elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(kern_entry32)

KERNEL_BASE = 0xFFFF800000000000;

SECTIONS {
    /* Load the kernel at this address: "." means the current address */
	. = 0x00200000;

    .entry32 : {
        obj/kern/init/entry32.o*(.text .rodata .data)
    }

    . = ALIGN(0x1000);

    . += KERNEL_BASE;

    .text : AT(ADDR(.text) - KERNEL_BASE) {
        *(.text .stub .text.* .gnu.linkonce.t.*)
    }

    PROVIDE(etext = . - KERNEL_BASE); /* Define the 'etext' symbol to this value */

    .rodata : AT(ADDR(.rodata) - KERNEL_BASE) {
        *(.rodata .rodata.* .gnu.linkonce.r.*)
    }

    /* Adjust the address for the data segment to the next page */
    . = ALIGN(0x1000);

    /* The data segment */
    .data : AT(ADDR(.data) - KERNEL_BASE) {
        *(.data)
    }

    PROVIDE(edata = . - KERNEL_BASE);

    .bss : AT(ADDR(.bss) - KERNEL_BASE) {
        *(.bss)
    }

    PROVIDE(end = . - KERNEL_BASE);

    /DISCARD/ : {
        *(.eh_frame .note.GNU-stack)
    }
}

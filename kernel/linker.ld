OUTPUT_FORMAT("elf32-i386")
ENTRY(_start)

SECTIONS {
    . = 0x1000;
    .text : {
        *(.text._start)
        *(.text*)
    }
    .rodata : { *(.rodata*) }
    .data : { *(.data) }
    .bss : {
        *(.bss)
        *(COMMON)
    }
}

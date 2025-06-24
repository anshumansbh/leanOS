/* kernel/linker.ld - Linker script for myos kernel */
OUTPUT_FORMAT("binary")
ENTRY(_start)

SECTIONS
{
  /* Load kernel at 0x1000 — must match boot.asm */
  . = 0x1000;

  .text : {
    *(.text)
  }

  .data : {
    *(.data)
  }

  .bss : {
    *(.bss COMMON)
  }
}

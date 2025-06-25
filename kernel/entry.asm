; kernel/entry.asm - 32-bit entry stub for kernel
BITS 32
SECTION .text._start
GLOBAL _start

extern kernel_main

_start:
    cli
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x9FF00
    call kernel_main
    

.hang:
    hlt
    jmp .hang
    
    
    ;jmp $
    ;hlt 
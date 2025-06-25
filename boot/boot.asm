; boot/boot.asm - 16-bit bootloader for 32-bit C kernel
org 0x7C00
bits 16

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    mov si, msg_boot
    call print_string

    ; Enable A20
    mov si, msg_a20
    call print_string
    call enable_a20

    ; Load kernel (8 sectors) to 0x1000
    mov si, msg_load
    call print_string
    mov ax, 0x0000
    mov es, ax
    mov bx, 0x1000
    mov ah, 0x02
    mov al, 8
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [BOOT_DRIVE]
    int 0x13
    jc disk_error

    ; Set up GDT
    mov si, msg_gdt
    call print_string
    call setup_gdt

    ; Switch to protected mode
    mov si, msg_pm
    call print_string
    cli
    lgdt [GDT_DESC]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:protected_mode

; 32-bit protected mode
bits 32
protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x9FF00
    cli
    ; Minimal IDT
    lea eax, [idt]
    mov [idtr+2], eax
    lidt [idtr]
    jmp 0x08:0x1000

idt: times 8 db 0
idtr: dw 7
      dd idt

; BIOS teletype print
bits 16
print_string:
    mov ah, 0x0E
.next:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .next
.done:
    ret

enable_a20:
    in al, 0x92
    or al, 2
    out 0x92, al
    ret

GDT:
    dq 0x0000000000000000
    dq 0x00CF9A000000FFFF
    dq 0x00CF92000000FFFF
GDT_DESC:
    dw GDT_end - GDT - 1
    dd GDT
GDT_end:

setup_gdt:
    ret

disk_error:
    mov si, msg_diskerr
    call print_string
    jmp $

msg_boot     db "Bootloader started",0x0D,0x0A,0
msg_a20      db "Enabling A20",0x0D,0x0A,0
msg_load     db "Loading kernel",0x0D,0x0A,0
msg_gdt      db "Setting up GDT",0x0D,0x0A,0
msg_pm       db "Switching to protected mode",0x0D,0x0A,0
msg_diskerr  db "Disk read error!",0x0D,0x0A,0

BOOT_DRIVE: db 0

times 510-($-$$) db 0
    db 0x55, 0xAA

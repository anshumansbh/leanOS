; boot/boot.asm - 16-bit bootloader for myos
; Loads kernel to 0x1000, switches to 32-bit protected mode, jumps to kernel

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

    ; Load the kernel BEFORE switching to protected mode!!!
    mov si, msg_load
    call print_string
    call load_kernel       

    ; Set up GDT
    mov si, msg_gdt
    call print_string
    call setup_gdt

    ; Enter PM
    mov si, msg_pm
    call print_string
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:protected_mode


    ; Load kernel (sectors 2+) to 0x1000
    mov si, msg_load
    call print_string
    mov ax, 0x1000
    mov es, ax
    xor bx, bx
    mov ah, 0x02          ; INT 13h: Read sectors
    mov al, 8             ; Number of sectors (adjust as needed)
    mov ch, 0
    mov cl, 2             ; Start at sector 2
    mov dh, 0
    mov dl, [BOOT_DRIVE]
    int 0x13
    jc disk_error
    ret

    ; Diagnostic: dump kernel bytes (disabled for now)

    ; After loading kernel
    mov si, msg_dump
    call print_string
    mov bx, 0x1000
    mov cx, 16
print_bytes:
    ; mov al, [bx]
    ; call print_hex_byte
    mov al, ' '
    ; call print_char
    inc bx
    loop print_bytes

    ; Far jump to 32-bit kernel at 0x1000
    mov si, msg_jump
    call print_string
    jmp $

; --------------------------
; Print string at DS:SI using BIOS teletype (INT 10h, AH=0Eh)
; --------------------------
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

; --------------------------
; Enable A20 line (fast method)
; --------------------------
enable_a20:
    in al, 0x92
    or al, 2
    out 0x92, al
    ret

; --------------------------
; GDT setup
; --------------------------
GDT:
    dq 0x0000000000000000      ; Null descriptor
    dq 0x00CF9A000000FFFF      ; Code segment: base=0, limit=4GB, 32-bit, RX
    dq 0x00CF92000000FFFF      ; Data segment: base=0, limit=4GB, 32-bit, RW
GDT_DESC:
    dw GDT_end - GDT - 1
    dd GDT
GDT_end:

setup_gdt:
    lgdt [GDT_DESC]
    ret

; --------------------------
; Enter protected mode
; --------------------------
enter_protected_mode:
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:protected_mode

; --------------------------
; 32-bit code starts here
; --------------------------
bits 32
protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000
    cli
    jmp 0x08:0x1000

; --------------------------
; Load kernel from disk (sectors 2+) to 0x1000
; --------------------------
bits 16
load_kernel:
    mov bx, 0x1000        ; ES:BX = 0x0000:0x1000
    mov es, bx
    xor bx, bx
    mov ah, 0x02          ; INT 13h: Read sectors
    mov al, 8             ; Number of sectors (adjust as needed)
    mov ch, 0
    mov cl, 2             ; Start at sector 2
    mov dh, 0
    mov dl, [BOOT_DRIVE]
    int 0x13
    jc disk_error
    ret

disk_error:
    mov si, msg_diskerr
    call print_string
    jmp $

; --------------------------
; Data and messages
; --------------------------
msg_boot    db "Bootloader started",0x0D,0x0A,0
msg_a20     db "Enabling A20",0x0D,0x0A,0
msg_gdt     db "Setting up GDT",0x0D,0x0A,0
msg_pm      db "Switching to protected mode",0x0D,0x0A,0
msg_load    db "Loading kernel",0x0D,0x0A,0
msg_diskerr db "Disk read error!",0x0D,0x0A,0
msg_jump    db "Jumping to kernel...", 0x0D, 0x0A, 0
msg_after_jump db "Returned from kernel jump!", 0x0D, 0x0A, 0
msg_dump    db "Kernel @0x1000: ", 0

BOOT_DRIVE: db 0

; --------------------------
; Boot signature and padding
; --------------------------
times 510-($-$$) db 0
    db 0x55, 0xAA 
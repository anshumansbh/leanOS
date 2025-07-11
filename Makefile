ASM = nasm
CC = i686-elf-gcc
LD = i686-elf-ld
OBJCOPY = i686-elf-objcopy

CFLAGS = -ffreestanding -m32 -nostdlib -O2 -Wall -Wextra
LDFLAGS = -T kernel/linker.ld

BUILD = build
BOOT_BIN = $(BUILD)/boot.bin
ENTRY_OBJ = $(BUILD)/entry.o
KERNEL_OBJ = $(BUILD)/kernel.o
VGA_OBJ = $(BUILD)/vga.o
KEYBOARD_OBJ = $(BUILD)/keyboard.o
IDT_OBJ = $(BUILD)/idt.o
IDT_C_OBJ = $(BUILD)/idt_c.o
KERNEL_ELF = $(BUILD)/kernel.elf
KERNEL_BIN = $(BUILD)/kernel.bin
OS_IMG = $(BUILD)/os.img
SHELL_OBJ = $(BUILD)/shell.o

all: $(OS_IMG)

$(BUILD):
	mkdir -p $(BUILD)

$(BOOT_BIN): boot/boot.asm | $(BUILD)
	$(ASM) -f bin $< -o $@

$(ENTRY_OBJ): kernel/entry.asm | $(BUILD)
	$(ASM) -f elf32 $< -o $@

$(KERNEL_OBJ): kernel/kernel.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(VGA_OBJ): src/drivers/vga.c src/drivers/vga.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(KEYBOARD_OBJ): src/drivers/keyboard.c src/drivers/keyboard.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(IDT_OBJ): src/interrupts/idt.asm | $(BUILD)
	$(ASM) -f elf32 $< -o $@

$(IDT_C_OBJ): src/interrupts/idt.c src/interrupts/idt.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(SHELL_OBJ): src/shell/shell.c src/shell/shell.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_ELF): $(ENTRY_OBJ) $(KERNEL_OBJ) $(VGA_OBJ) $(KEYBOARD_OBJ) $(IDT_OBJ) $(IDT_C_OBJ) $(SHELL_OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@

$(OS_IMG): $(BOOT_BIN) $(KERNEL_BIN)
	cat $^ > $@
	truncate -s 4608 $@

run: $(OS_IMG)
	qemu-system-i386 -drive format=raw,file=$(OS_IMG),if=floppy

clean:
	rm -rf $(BUILD)

.PHONY: all run clean

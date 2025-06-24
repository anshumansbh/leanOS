# Top-level Makefile for myos

BOOT = boot/boot.asm
KERNEL = kernel/kernel.c
VGA = kernel/vga.c
LINKER = kernel/linker.ld
BUILD = build
BOOT_BIN = $(BUILD)/boot.bin
KERNEL_BIN = $(BUILD)/kernel.bin
OS_IMG = $(BUILD)/os.img

AS = nasm
CC = /opt/cross/bin/i686-elf-gcc
LD = /opt/cross/bin/i686-elf-ld

CFLAGS = -ffreestanding -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -Wall -Wextra
LDFLAGS = -T $(LINKER) -nostdlib

KERNEL_OBJS = $(BUILD)/kernel.o $(BUILD)/vga.o

all: $(OS_IMG)

$(BUILD):
	mkdir -p $(BUILD)

$(BOOT_BIN): $(BOOT) | $(BUILD)
	$(AS) -f bin $< -o $@

$(BUILD)/kernel.o: $(KERNEL) kernel/vga.h | $(BUILD)
	$(CC) $(CFLAGS) -c $(KERNEL) -o $@

$(BUILD)/vga.o: $(VGA) kernel/vga.h | $(BUILD)
	$(CC) $(CFLAGS) -c $(VGA) -o $@

$(KERNEL_BIN): $(KERNEL_OBJS) $(LINKER) | $(BUILD)
	$(LD) $(LDFLAGS) -o $@ $(KERNEL_OBJS)

$(OS_IMG): $(BOOT_BIN) $(KERNEL_BIN)
	dd if=/dev/zero of=$@ bs=512 count=2880
	dd if=$(BOOT_BIN) of=$@ conv=notrunc
	dd if=$(KERNEL_BIN) of=$@ bs=512 seek=1 conv=notrunc

run: $(OS_IMG)
	qemu-system-i386 -fda $(OS_IMG)

clean:
	rm -rf $(BUILD)

.PHONY: all run clean

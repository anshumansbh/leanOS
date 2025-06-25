// kernel/kernel.c - Minimal 32-bit kernel for myos
// Uses VGA logger for colored output

#include "vga.h"

void kernel_main(void) __attribute__((noreturn));
void kernel_main(void) {
    char* video = (char*) 0xB8000;
    const char* msg = "Kernel loaded!";
    for (int i = 0; msg[i] != '\0'; ++i) {
        video[i * 2] = msg[i];
        video[i * 2 + 1] = 0x07;
    }
    while (1) {}
}


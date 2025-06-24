// kernel/kernel.c - Minimal 32-bit kernel for myos
// Uses VGA logger for colored output

#include "vga.h"

void kernel_main(void) __attribute__((noreturn));
void kernel_main(void) {
    clear_screen();
    log("[INFO] Kernel start\n");
    log_color("[DEBUG] Initializing VGA...\n", LOG_COLOR_DEBUG);
    log_color("[INFO] VGA ready\n", LOG_COLOR_INFO);
    log_color("[WARN] This is a warning\n", LOG_COLOR_WARN);
    log_color("[ERROR] This is an error\n", LOG_COLOR_ERROR);
    log_color("[DEBUG] Kernel loaded at address: ", LOG_COLOR_DEBUG);
    log_hex((unsigned int)kernel_main);
    log("\n");
    volatile char* vga = (volatile char*)0xB8000;
    vga[0] = 'K';
    vga[1] = 0x0F;
    while (1) { __asm__("hlt"); }
}

// Assembly stub to call kernel_main
__asm__(
    ".global _start\n"
    "_start:\n"
    "call kernel_main\n"
    "cli\n"
    "hlt\n"
); 
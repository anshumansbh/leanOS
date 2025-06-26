// kernel/kernel.c - Minimal 32-bit kernel for myos
// Uses VGA logger for colored output

#include "vga.h"
#include "keyboard.h"
#include "idt.h"

static char scancode_to_ascii[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' ', 0,
    // rest are zeros
};

static unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void kernel_main(void) __attribute__((noreturn));
void kernel_main(void) {
    idt_init();
    clear_screen();
    log_color("Kernel loaded!\n", LOG_COLOR_INFO);
    log_color("Type keys. Backspace supported. Press ESC to halt.\n", LOG_COLOR_DEBUG);
    keyboard_init();

    while (1) {
        keyboard_poll();
        char c = keyboard_getchar();
        if (c) {
            if (c == 27) { // ESC
                log_color("\n[INFO] Halting.\n", LOG_COLOR_INFO);
                break;
            } else if (c == '\b') {
                log_color("\b \b", LOG_COLOR_INFO); // Erase char visually
            } else if (c == '\n') {
                log_color("\n", LOG_COLOR_INFO);
                // Optionally: process the line here
                keyboard_clear_buffer();
            } else {
                char msg[2] = {c, 0};
                log_color(msg, LOG_COLOR_WARN);
            }
        }
    }
    while (1) { __asm__("hlt"); }
}


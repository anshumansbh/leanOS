#include "../drivers/vga.h"
#include "../drivers/keyboard.h"
#include "shell.h"

static void print_prompt() {
    log_color("leanOS> ", LOG_COLOR_DEBUG);
}

static int strcmp(const char *a, const char *b) {
    while (*a && (*a == *b)) { a++; b++; }
    return *(const unsigned char*)a - *(const unsigned char*)b;
}

static void readline(char *buf, int max_len) {
    int i = 0;
    keyboard_clear_buffer();
    
    while (i < max_len - 1) {
        if (keyboard_poll()) {
            char c = keyboard_getchar();
            if (c == '\n' || c == '\r') {
                buf[i] = '\0';
                log("\n");
                return;
            } else if (c == '\b' && i > 0) {
                i--;
                log("\b \b");
            } else if (c >= 32 && c <= 126) {
                buf[i++] = c;
                char str[2] = {c, '\0'};
                log(str);
            }
        }
    }
    buf[i] = '\0';
}

void shell_main(void) {
    char buf[256];
    while (1) {
        print_prompt();
        readline(buf, sizeof(buf));
        if (strcmp(buf, "help") == 0) {
            log("Available commands: help, clear, halt\n");
        } else if (strcmp(buf, "clear") == 0) {
            clear_screen();
        } else if (strcmp(buf, "halt") == 0) {
            log("Halting...\n");
            __asm__("cli; hlt");
        } else if (buf[0] != '\0') {
            log("Unknown command. Type 'help'.\n");
        }
    }
} 
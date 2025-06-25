// kernel/keyboard.c - PS/2 keyboard polling and ASCII conversion
#include "keyboard.h"

// Simple US QWERTY scancode to ASCII table
static const char scancode_to_ascii[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' ', 0,
    // rest are zeros
};

static char last_char = 0;

static unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void keyboard_init(void) {
    last_char = 0;
}

// Poll the keyboard, process scancode, and buffer ASCII char
int keyboard_poll(void) {
    unsigned char status = inb(0x64);
    if (status & 1) {
        unsigned char sc = inb(0x60);
        static int shift = 0;
        if (sc == 0x2A || sc == 0x36) { shift = 1; return 0; } // Shift down
        if (sc == 0xAA || sc == 0xB6) { shift = 0; return 0; } // Shift up
        if (sc == 0x0E) { last_char = '\b'; return 1; } // Backspace
        if (sc < 128 && scancode_to_ascii[sc]) {
            char c = scancode_to_ascii[sc];
            // TODO: Add shift support for uppercase
            last_char = c;
            return 1;
        }
    }
    return 0;
}

// Get the last ASCII character typed (0 if none)
char keyboard_getchar(void) {
    char c = last_char;
    last_char = 0;
    return c;
} 
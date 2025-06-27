// kernel/keyboard.c - Robust PS/2 keyboard polling and line editing
#include "keyboard.h"
#include "vga.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define KB_BUF_SIZE (VGA_WIDTH * VGA_HEIGHT)

// US QWERTY scancode to ASCII tables
static const char scancode_to_ascii[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' ', 0,
    // rest are zeros
};
static const char scancode_to_ascii_shift[128] = {
    0, 27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n', 0,
    'A','S','D','F','G','H','J','K','L',':','"','~', 0,'|',
    'Z','X','C','V','B','N','M','<','>','?', 0, '*', 0, ' ', 0,
    // rest are zeros
};

static char last_char = 0;
static unsigned char modifiers = 0; // bit 0: shift, bit 1: capslock
static char buffer[KB_BUF_SIZE];
static int buf_len = 0;

static unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void keyboard_init(void) {
    last_char = 0;
    modifiers = 0;
    buf_len = 0;
    buffer[0] = 0;
}

void keyboard_panic(const char* msg) {
    log_color("\n[KEYBOARD ERROR] ", LOG_COLOR_ERROR);
    log_color(msg, LOG_COLOR_ERROR);
    log_color("\System halting.\n", LOG_COLOR_ERROR);
    vga_flash_screen();
}

// Poll the keyboard, process scancode, and buffer ASCII char
int keyboard_poll(void) {
    unsigned char status = inb(0x64);
    if (status & 1) {
        unsigned char sc = inb(0x60);
        // Shift
        if (sc == 0x2A || sc == 0x36) { modifiers |= 1; return 0; } // Shift down
        if (sc == 0xAA || sc == 0xB6) { modifiers &= ~1; return 0; } // Shift up
        // Caps Lock
        if (sc == 0x3A) { modifiers ^= 2; return 0; } // Toggle capslock
        // Only process key press (not release)
        if (sc & 0x80) return 0;
        // Backspace
        if (sc == 0x0E) {
            if (buf_len > 0) {
                buf_len--;
                buffer[buf_len] = 0;
                last_char = '\b';
                return 1;
            }
            last_char = 0;
            return 0;
        }
        // Tab
        if (sc == 0x0F) {
            int spaces = 4;
            while (spaces-- && buf_len < KB_BUF_SIZE-1) {
                buffer[buf_len++] = ' ';
            }
            buffer[buf_len] = 0;
            last_char = 0;
            return 1;
        }
        if (sc < 128) {
            char c;
            if (modifiers & 1) {
                c = scancode_to_ascii_shift[sc];
            } else {
                c = scancode_to_ascii[sc];
            }
            // Caps lock for letters
            if ((modifiers & 2) && c >= 'a' && c <= 'z') {
                c = c - 'a' + 'A';
            } else if ((modifiers & 2) && c >= 'A' && c <= 'Z') {
                c = c - 'A' + 'a';
            }
            if (c && c != '\b' && c != '\t') {
                if (c == '\n') {
                    buffer[buf_len] = 0;
                } else if (buf_len < KB_BUF_SIZE-1) {
                    buffer[buf_len++] = c;
                    buffer[buf_len] = 0;
                } else {
                    last_char = 0;
                    return 0;
                }
                last_char = c;
                return 1;
            }
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

// Get the current modifier state (bit 0: shift, bit 1: capslock)
unsigned char keyboard_modifiers(void) {
    return modifiers;
}

// Get the current input buffer (null-terminated)
const char* keyboard_buffer(void) {
    return buffer;
}

// Clear the input buffer
void keyboard_clear_buffer(void) {
    buf_len = 0;
    buffer[0] = 0;
} 
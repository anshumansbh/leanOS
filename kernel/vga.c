// kernel/vga.c - VGA text mode logger implementation
// Provides log, log_color, log_hex, clear_screen, and scrolling

#include "vga.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((volatile char*)0xB8000)

// Global cursor position
static int cursor_row = 0;
static int cursor_col = 0;

// Set hardware cursor (optional, not required for QEMU)
static void update_cursor() {
    // unsigned short pos = cursor_row * VGA_WIDTH + cursor_col;
    // Outb to VGA ports 0x3D4 and 0x3D5 (not required for QEMU text output)
}

// Scroll the screen up by one line
static void scroll() {
    int row, col;
    for (row = 1; row < VGA_HEIGHT; ++row) {
        for (col = 0; col < VGA_WIDTH; ++col) {
            int from = (row * VGA_WIDTH + col) * 2;
            int to = ((row - 1) * VGA_WIDTH + col) * 2;
            VGA_MEMORY[to] = VGA_MEMORY[from];
            VGA_MEMORY[to + 1] = VGA_MEMORY[from + 1];
        }
    }
    // Clear last line
    for (col = 0; col < VGA_WIDTH; ++col) {
        int idx = ((VGA_HEIGHT - 1) * VGA_WIDTH + col) * 2;
        VGA_MEMORY[idx] = ' ';
        VGA_MEMORY[idx + 1] = LOG_COLOR_INFO;
    }
    if (cursor_row > 0) cursor_row--;
    if (cursor_row < 0) vga_panic("Scroll: cursor_row < 0");
}

// Clear the VGA text screen
void clear_screen(void) {
    int row, col;
    for (row = 0; row < VGA_HEIGHT; ++row) {
        for (col = 0; col < VGA_WIDTH; ++col) {
            int idx = (row * VGA_WIDTH + col) * 2;
            VGA_MEMORY[idx] = ' ';
            VGA_MEMORY[idx + 1] = LOG_COLOR_INFO;
        }
    }
    cursor_row = 0;
    cursor_col = 0;
    update_cursor();
}

// Print a character at the current cursor position with color
static void putchar(char c, unsigned char color) {
    if (c == '\n') {
        cursor_col = 0;
        cursor_row++;
    } else if (c == '\r') {
        cursor_col = 0;
    } else if (c == '\b') {
        if (cursor_col > 0) {
            cursor_col--;
            int idx = (cursor_row * VGA_WIDTH + cursor_col) * 2;
            VGA_MEMORY[idx] = ' ';
            VGA_MEMORY[idx + 1] = color;
        }
    } else {
        int idx = (cursor_row * VGA_WIDTH + cursor_col) * 2;
        VGA_MEMORY[idx] = c;
        VGA_MEMORY[idx + 1] = color;
        cursor_col++;
        if (cursor_col >= VGA_WIDTH) {
            cursor_col = 0;
            cursor_row++;
        }
    }
    if (cursor_row >= VGA_HEIGHT) {
        scroll();
    }
    if (cursor_row < 0 || cursor_row >= VGA_HEIGHT) {
        vga_panic("Cursor row out of bounds");
    }
    if (cursor_col < 0 || cursor_col >= VGA_WIDTH) {
        vga_panic("Cursor col out of bounds");
    }
    update_cursor();
}

// Print a string in a specific color
void log_color(const char* msg, unsigned char color) {
    for (int i = 0; msg[i] != '\0'; ++i) {
        putchar(msg[i], color);
    }
}

// Print a string in default color (INFO)
void log(const char* msg) {
    log_color(msg, LOG_COLOR_INFO);
}

// Print a 32-bit value in hexadecimal
void log_hex(unsigned int value) {
    char hex_chars[] = "0123456789ABCDEF";
    log_color("0x", LOG_COLOR_DEBUG);
    for (int i = 28; i >= 0; i -= 4) {
        char c = hex_chars[(value >> i) & 0xF];
        putchar(c, LOG_COLOR_DEBUG);
    }
}

void vga_flash_screen(void) {
    // Save current VGA buffer
    char saved[VGA_WIDTH * VGA_HEIGHT * 2];
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT * 2; ++i) {
        saved[i] = VGA_MEMORY[i];
    }
    // Flash red
    for (int f = 0; f < 2; ++f) {
        unsigned char color = 0x4C; // Red on black
        for (int row = 0; row < VGA_HEIGHT; ++row) {
            for (int col = 0; col < VGA_WIDTH; ++col) {
                int idx = (row * VGA_WIDTH + col) * 2;
                VGA_MEMORY[idx] = ' ';
                VGA_MEMORY[idx + 1] = color;
            }
        }
        for (volatile int d = 0; d < 2000000; ++d) { __asm__("nop"); }
    }
    // Restore previous screen
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT * 2; ++i) {
        VGA_MEMORY[i] = saved[i];
    }
}


void vga_panic(const char* msg) {
    log_color("\n[VGA ERROR] ", LOG_COLOR_ERROR);
    log_color(msg, LOG_COLOR_ERROR);
    log_color("\System halting.\n", LOG_COLOR_ERROR);
    vga_flash_screen();
}

volatile char* vga = (volatile char*)0xB8000;
// vga[0] = 'H';
// vga[1] = 0x0F; 
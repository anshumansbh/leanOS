// kernel/vga.h - VGA text mode logger interface
// Provides log, log_color, log_hex, clear_screen, and log level color constants

#ifndef VGA_H
#define VGA_H

// VGA color constants for log levels
#define LOG_COLOR_INFO   0x0F // White on black
#define LOG_COLOR_WARN   0x0E // Yellow on black
#define LOG_COLOR_ERROR  0x0C // Red on black
#define LOG_COLOR_DEBUG  0x0B // Cyan on black

// Print a message in default color (INFO)
void log(const char* msg);

// Print a message in a specific color
void log_color(const char* msg, unsigned char color);

// Print a 32-bit value in hexadecimal
void log_hex(unsigned int value);

// Clear the VGA text screen
void clear_screen(void);

#endif // VGA_H 
// kernel/keyboard.h - Robust keyboard input interface
// Provides polling, buffer, and character retrieval for PS/2 keyboard

#ifndef KEYBOARD_H
#define KEYBOARD_H

// Initialize keyboard (resets buffer and state)
void keyboard_init(void);

// Poll the keyboard, returns 1 if a key was processed
int keyboard_poll(void);

// Get the last ASCII character typed (0 if none)
char keyboard_getchar(void);

// Get the current modifier state (bit 0: shift, bit 1: capslock)
unsigned char keyboard_modifiers(void);

// Get the current input buffer (null-terminated)
const char* keyboard_buffer(void);

// Clear the input buffer
void keyboard_clear_buffer(void);

// Log a fatal keyboard error and halt the system
void keyboard_panic(const char* msg);

#endif // KEYBOARD_H 
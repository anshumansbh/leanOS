// kernel/keyboard.h - Keyboard input interface
// Provides polling and character retrieval for PS/2 keyboard

#ifndef KEYBOARD_H
#define KEYBOARD_H

// Initialize keyboard (currently a no-op for polling)
void keyboard_init(void);

// Poll the keyboard, returns 1 if a key was processed
int keyboard_poll(void);

// Get the last ASCII character typed (0 if none)
char keyboard_getchar(void);

#endif // KEYBOARD_H 
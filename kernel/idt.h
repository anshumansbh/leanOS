// kernel/idt.h - IDT and exception handler interface
#ifndef IDT_H
#define IDT_H

// Initialize the IDT and install exception handlers
void idt_init(void);

// C exception handler called from stubs (prints/logs and halts)
void exception_handler_c(int num);

#endif // IDT_H 
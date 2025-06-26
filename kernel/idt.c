#include <stdint.h>
#include "idt.h"
#include "vga.h"

struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

extern void exc_stub_0(void);
extern void exc_stub_1(void);
extern void exc_stub_2(void);
extern void exc_stub_3(void);
extern void exc_stub_4(void);
extern void exc_stub_5(void);
extern void exc_stub_6(void);
extern void exc_stub_7(void);
extern void exc_stub_8(void);
extern void exc_stub_9(void);
extern void exc_stub_10(void);
extern void exc_stub_11(void);
extern void exc_stub_12(void);
extern void exc_stub_13(void);
extern void exc_stub_14(void);
extern void exc_stub_15(void);
extern void exc_stub_16(void);
extern void exc_stub_17(void);
extern void exc_stub_18(void);
extern void exc_stub_19(void);
extern void exc_stub_20(void);
extern void exc_stub_21(void);
extern void exc_stub_22(void);
extern void exc_stub_23(void);
extern void exc_stub_24(void);
extern void exc_stub_25(void);
extern void exc_stub_26(void);
extern void exc_stub_27(void);
extern void exc_stub_28(void);
extern void exc_stub_29(void);
extern void exc_stub_30(void);
extern void exc_stub_31(void);

static struct idt_entry idt[256];

static void idt_set_gate(int n, void (*handler)()) {
    uint32_t addr = (uint32_t)handler;
    idt[n].offset_low = addr & 0xFFFF;
    idt[n].selector = 0x08;
    idt[n].zero = 0;
    idt[n].type_attr = 0x8E;
    idt[n].offset_high = (addr >> 16) & 0xFFFF;
}

void idt_init(void) {
    void (*stubs[32])(void) = {
        exc_stub_0, exc_stub_1, exc_stub_2, exc_stub_3, exc_stub_4, exc_stub_5, exc_stub_6, exc_stub_7,
        exc_stub_8, exc_stub_9, exc_stub_10, exc_stub_11, exc_stub_12, exc_stub_13, exc_stub_14, exc_stub_15,
        exc_stub_16, exc_stub_17, exc_stub_18, exc_stub_19, exc_stub_20, exc_stub_21, exc_stub_22, exc_stub_23,
        exc_stub_24, exc_stub_25, exc_stub_26, exc_stub_27, exc_stub_28, exc_stub_29, exc_stub_30, exc_stub_31
    };
    for (int i = 0; i < 32; ++i) {
        idt_set_gate(i, stubs[i]);
    }
    struct idt_ptr idtp = { sizeof(idt) - 1, (uint32_t)idt };
    __asm__ volatile ("lidtl (%0)" : : "r"(&idtp));
}

static const char* exc_names[32] = {
    "Divide by zero", "Debug", "NMI", "Breakpoint", "Overflow", "Bound range", "Invalid opcode", "Device not available",
    "Double fault", "Coprocessor segment", "Invalid TSS", "Segment not present", "Stack fault", "General protection",
    "Page fault", "Reserved", "x87 FPU", "Alignment check", "Machine check", "SIMD FP", "Virtualization", "Control protection",
    "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved"
};

void exception_handler_c(int num) {
    log_color("\n[EXCEPTION] ", LOG_COLOR_ERROR);
    if (num >= 0 && num < 32) {
        log_color(exc_names[num], LOG_COLOR_ERROR);
    } else {
        log_color("Unknown", LOG_COLOR_ERROR);
    }
    log_color(" (", LOG_COLOR_ERROR);
    log_hex(num);
    log_color(")\System Halting.\n", LOG_COLOR_ERROR);
    vga_flash_screen();
} 
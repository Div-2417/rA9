#include "idt.h"
#include "isr.h"
#include "kprintf.h"
#include "drivers/io.h"
#include <stdint.h>

static const char* exception[32]={
    "Divide Error", "Debug", "NMI", "Breakpoint", "Overflow",
    "Bound Range", "Invalid Opcode", "Device Not Available",
    "Double Fault", "Coprocessor Overrun", "Invalid TSS",
    "Segment Not Present", "Stack Fault", "General Protection",
    "Page Fault", "Reserved", "x87 FPU Error", "Alignment Check",
    "Machine Check", "SIMD FP Exception", "Virtualization",
    "Control Protection", "Reserved", "Reserved", "Reserved",
    "Reserved", "Reserved", "Reserved", "Hypervisor Injection",
    "VMM Communication", "Security Exception", "Reserved"

};

struct isrFrame{
    uint32_t gs,fs,es,ds;
    uint32_t edi,esi,ebp, espDummy,ebx,ecx,eax;
    uint32_t iniNo, errCode;
    uint32_t eip,cs,eFlags;
} __attribute__((packed));

static void (*irq_handlers[16])(void) = {0};

void register_interrupt_handler(uint8_t irq, void (*handler)(void)) {
    if (irq < 16) irq_handlers[irq] = handler;
}

void pic_remap(void) {
    outb(0x20, 0x11);  outb(0xA0, 0x11);
    outb(0x21, 0x20);  outb(0xA1, 0x28);
    outb(0x21, 0x04);  outb(0xA1, 0x02);
    outb(0x21, 0x01);  outb(0xA1, 0x01);
    outb(0x21, 0xFF);  outb(0xA1, 0xFF);   // mask all
}

void isr_handler(struct isrFrame* frame) {
    uint32_t vector = frame->iniNo;
    uint32_t error  = frame->errCode;

    if (vector < 32) {
        kprintf("Exception %u: %s\n", vector, exception[vector]);
        if (vector == 8 || (vector >= 10 && vector <= 14) ||
            vector == 17 || vector == 21 || vector == 29 || vector == 30) {
            kprintf("Error code: 0x%x\n", error);
        }
        for (;;) asm volatile("cli; hlt");
    } else if (vector >= 32 && vector < 48) {
        uint8_t irq = vector - 32;
        if (irq_handlers[irq]) irq_handlers[irq]();
        if (irq >= 8) outb(0xA0, 0x20);
        outb(0x20, 0x20);
    }
}

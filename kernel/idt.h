#ifndef KERNEL_IDT_H
#define KERNEL_IDT_H

#include <stdint.h>

struct idtEntry {
   uint16_t offset_1;        // offset bits 0..15
   uint16_t selector;        // a code segment selector in GDT or LDT
   uint8_t  zero;            // unused, set to 0
   uint8_t  flags; // gate type, dpl, and p fields
   uint16_t offset_2;        // offset bits 16..31
}  __attribute__((packed));

struct idtPtr{
    uint16_t limit;
    uint32_t base;
}__attribute__((packed));

#define IDT_ENTRIES 256
#define IDT_FLAG_PRESENT 0x80
#define IDT_FLAG_GATE_INTERRUPT 0x0E

extern struct idtEntry idt[IDT_ENTRIES];
extern struct idtPtr idtp;

void idtSetGate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void idtInstall(void);
extern void idtLoad(const void *idtr);

#endif
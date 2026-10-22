#include "idt.h"
#include <stdint.h>

extern uint32_t isr_stub_table[];

struct idtEntry idt[IDT_ENTRIES];
struct idtPtr idtp;

void idtSetGate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags){

    idt[num].offset_1 = base & 0xFFFF;
    idt[num].selector = sel;
    idt[num].zero = 0;
    idt[num].flags = flags;
    idt[num].offset_2 = (base >> 16) & 0xFFFF;
}

void idtInstall(void){

    idtp.limit = sizeof(struct idtEntry) * IDT_ENTRIES -1;
    idtp.base = (uint32_t)&idt;

    for (int i = 0; i < IDT_ENTRIES; i++) {
        if (isr_stub_table[i])
            idtSetGate(i, isr_stub_table[i], 0x08, 0x8E);
        else
            idtSetGate(i, 0, 0, 0);
    }

    idtLoad(&idtp);
}

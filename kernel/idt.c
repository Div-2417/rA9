#include "idt.h"
#include <stdint.h>

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

    //clear idt
    for(int i =0; i< IDT_ENTRIES; i++){
        idtSetGate(i,0,0,0);
    }

    idtLoad(&idtp);
}
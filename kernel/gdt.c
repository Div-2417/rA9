#include "gdt.h"
#include <stdint.h>

//gdt[0] null descriptor,  gdt[1] kernel code segment,  gdt[2] kernel data segment
struct gdtEntry gdt[3];

struct gdtPtr gdtPtr;

void gdtEncode(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags){

    gdt[index].baseLow = (uint16_t)(base & 0xFFFF);
    gdt[index].baseMid = (int8_t)((base >> 16) & 0xFF);
    gdt[index].baseHigh = (uint8_t)((base >> 24) & 0xFF);

    gdt[index].granularity = (uint8_t)((limit >>16) & 0x0F) | ((flags & 0x0F)<< 4);

    gdt[index].limitLow = (uint16_t)(limit & 0xFFFF);

    gdt[index].access = access;
}

void gdtInstall(void){

    gdtPtr.limit = (uint16_t)sizeof(gdt) - 1;
    gdtPtr.base = (uint32_t) &gdt;

    gdtEncode(0,0,0,0,0);

    gdtEncode(1,0,0xFFFFF,0x9A,0xC);

    gdtEncode(2,0,0xFFFFF,0x92,0xC);

    gdt_flush((uint32_t)&gdtPtr);
}
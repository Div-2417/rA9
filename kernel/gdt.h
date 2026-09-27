#ifndef KERNEL_GDT_H
#define KERNEL_GDT_H

#include <stdint.h>

struct gdtEntry{
    uint16_t limitLow;
    uint16_t baseLow;
    uint8_t baseMid;
    uint8_t access;
    uint8_t granularity;
    uint8_t baseHigh;
}__attribute__((packed));

struct gdtPtr{
    uint16_t limit;
    uint32_t base;
}__attribute__((packed));

extern void gdt_flush(uint32_t);

void gdtInstall(void);

#endif

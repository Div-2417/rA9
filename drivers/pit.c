#include "io.h"
#include "pit.h"
#include "kernel/isr.h"
#include <stdint.h>

#define PIT_BASE_HZ 1193182u

static volatile uint32_t ticks = 0;
static uint32_t tick_hz = 100;

static void pit_handler(void) {
    ticks++;
}

void pitInit(uint32_t hz){
   if (hz == 0) hz = 100;

   //compare with osdev asm implementation for understanding (not fully same btw)
    uint32_t div = (PIT_BASE_HZ + hz / 2) / hz;   // round to nearest
    if (div < 2) div = 2;                         // N=1 illegal in mode 2/3
    if (div > 65536) div = 65536;                 // 65536 encoded as 0

    tick_hz = (PIT_BASE_HZ + div / 2) / div;      // actual frequency

    outb(0x43, 0x36);                             // ch0, lo/hi, mode 3, binary
    outb(0x40, div & 0xFF);                       // 65536 -> 0x00
    outb(0x40, (div >> 8) & 0xFF);                // 65536 -> 0x00

    register_interrupt_handler(0, pit_handler);
    pic_unmask(0); 
}

uint32_t pitTicks(void){
    return ticks;
}

void pitSleep(uint32_t ms){
    uint32_t start = ticks;
    uint32_t need = (uint32_t)(((uint64_t)ms*tick_hz+999)/1000)+1; // +1 ensure cpu doesn't wake up early, affects accuracy tho resolution of 10

    while ((uint32_t)(ticks-start) < need){
        asm volatile("hlt");
    }
}
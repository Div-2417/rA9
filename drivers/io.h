#ifndef KERNEL_IO_H
#define KERNEL_IO_H

#include <stdint.h>

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void io_wait(void) {
    // write to unused port 0x80, gives ~1-4us delay
    // old hw needed this between successive out/in on same device
    __asm__ volatile ("outb %0, $0x80" : : "a"((uint8_t)0));
}

#endif
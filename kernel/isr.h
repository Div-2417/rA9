#ifndef KERNEL_ISR_H
#define KERNEL_ISR_H

#include <stdint.h>

void isrHandler(uint8_t irq, void (*handler)(void));
void pic_remap(void);
void idt_init_gates(void); 

#endif
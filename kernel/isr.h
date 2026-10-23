#ifndef KERNEL_ISR_H
#define KERNEL_ISR_H

#include <stdint.h>

#define PIC1 0x20		/* IO base address for master PIC */
#define PIC2 0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1+1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2+1)

#define PIC_CASCADE_IRQ 2

void register_interrupt_handler(uint8_t irq, void (*handler)(void));
void pic_remap(void);
void pic_unmask(uint8_t irq);

#endif
#include "drivers/io.h"
#include "kernel/idt.h"
#include <kernel/tty.h>
#include <kernel/kprintf.h>
#include <kernel/gdt.h>
#include <kernel/isr.h>

// cross compiler checks
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

void kernel_main(){
    InitTerminal();

    terminalPutString("hello \n MFs!!!");

    outb(0x3D4, 0x0F);
    uint8_t x = inb(0x3D5);

    gdtInstall();
    pic_remap();      // mask all IRQs
    idtInstall();    // install IDT (lidt)

    // Timer + keyboard init will go here, each unmasking its IRQ
    // register_interrupt_handler(0, timer_handler);  outb(0x21, inb(0x21) & ~0x01);
    // register_interrupt_handler(1, keyboard_handler); outb(0x21, inb(0x21) & ~0x02);

    asm volatile("sti");


    kprintf("Hello, %s!\n", "rA9");
    kprintf("Dec: %d, Neg: %d\n", 42, -7);
    kprintf("Hex: %x\n", 255);
    kprintf("Char: %c, Percent: %%\n", 'Z');
    kprintf("Zero: %d, Max hex: %x\n", 0, 4294967295u);
}
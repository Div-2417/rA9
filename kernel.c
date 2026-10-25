#include "drivers/pit.h"
#include "kernel/idt.h"
#include <kernel/tty.h>
#include <kernel/kprintf.h>
#include <kernel/gdt.h>
#include <kernel/isr.h>
#include "drivers/keyboard.h"

// cross compiler checks
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

void kernel_main(uint32_t magic, uint32_t addr){
    (void)magic; (void)addr;  // PMM will consume these later
    InitTerminal();

    terminalPutString("hello \n MFs!!!");

    gdtInstall();
    pic_remap();      // mask all IRQs
    idtInstall();    // install IDT (lidt)

    asm volatile("sti");

    // pitInit(100);
    // pitSleep(100);
    // kprintf("pitSleep done, ticks=%u\n", pitTicks());

    kbInit();
    kprintf("> ");

    kprintf("Hello, %s!\n", "rA9");
    kprintf("Dec: %d, Neg: %d\n", 42, -7);
    kprintf("Hex: %x\n", 255);
    kprintf("Char: %c, Percent: %%\n", 'Z');
    kprintf("Zero: %d, Max hex: %x\n", 0, 4294967295u);

    for (;;) {
    char c = kbReadChar();
    terminalPutChar(c);
    }

//    uint32_t last = 0;
//     for (;;) {
//     asm volatile("hlt");
//     uint32_t s = pitTicks() / 100;
//     if (s != last) {
//         last = s;
//         kprintf("uptime: %d s\n", (int)s);
//     }
//     } 
}

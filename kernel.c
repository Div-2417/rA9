#include "drivers/io.h"
#include "drivers/pit.h"
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

    gdtInstall();
    pic_remap();      // mask all IRQs
    idtInstall();    // install IDT (lidt)

    asm volatile("sti");

    kprintf("Hello, %s!\n", "rA9");
    kprintf("Dec: %d, Neg: %d\n", 42, -7);
    kprintf("Hex: %x\n", 255);
    kprintf("Char: %c, Percent: %%\n", 'Z');
    kprintf("Zero: %d, Max hex: %x\n", 0, 4294967295u);

   uint32_t last = 0;
    for (;;) {
    asm volatile("hlt");
    uint32_t s = pitTicks() / 100;
    if (s != last) {
        last = s;
        kprintf("uptime: %d s\n", (int)s);
    }
    } 
}

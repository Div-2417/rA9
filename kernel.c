#include "drivers/pit.h"
#include "kernel/idt.h"
#include <kernel/tty.h>
#include <kernel/kprintf.h>
#include <kernel/gdt.h>
#include <kernel/isr.h>
#include "drivers/keyboard.h"
#include "kernel/shell.h"
#include "kernel/pmm.h"

#include <stdint.h>

// cross compiler checks
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

multiboot_info_t* g_multiboot_info = NULL;
extern char _kernel_end[];

void kernel_main(uint32_t magic, uint32_t addr){
    (void)magic;
    g_multiboot_info = (multiboot_info_t*)addr;
    pmmInit(g_multiboot_info->mem_upper, (physicalAddr)_kernel_end);
    InitTerminal();

    terminalPutString("hello \n MFs!!!");

    gdtInstall();
    pic_remap();      // mask all IRQs
    idtInstall();    // install IDT (lidt)

    asm volatile("sti");

    kbInit();

    sLoop();
}
#include "drivers/pit.h"
#include "kernel/idt.h"
#include <kernel/tty.h>
#include <kernel/kprintf.h>
#include <kernel/gdt.h>
#include <kernel/isr.h>
#include "drivers/keyboard.h"
#include "kernel/shell.h"
#include "kernel/pmm.h"
#include "kernel/multiboot.h"

#include <stdint.h>

// cross compiler checks
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

multiboot_info_t* g_multiboot_info = NULL;
extern char _kernel_start[], _kernel_end[];

void kernel_main(uint32_t magic, uint32_t addr) {
    (void)magic;
    g_multiboot_info = (multiboot_info_t*)addr;

    pmmInit(g_multiboot_info->mem_upper, (physicalAddr)_kernel_end);

    if (g_multiboot_info->flags & (1 << 6)) {
        multiboot_mmap_entry_t* mmap = (multiboot_mmap_entry_t*)g_multiboot_info->mmap_addr;
        uint32_t mmap_end = g_multiboot_info->mmap_addr + g_multiboot_info->mmap_length;

        while ((uint32_t)mmap < mmap_end) {
            if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
                pmmInitRegion(mmap->addr, mmap->len);
            }
            mmap = (multiboot_mmap_entry_t*)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
        }
    }

    pmmDeInitRegion((physicalAddr)_kernel_start, (physicalAddr)_kernel_end - (physicalAddr)_kernel_start);
    pmmDeInitRegion(pmmGetBitmapAddr(), pmmGetBitmapSize());
    pmmDeInitRegion((physicalAddr)g_multiboot_info, sizeof(multiboot_info_t));

    InitTerminal();

    terminalSetColour(vga_entryColour(VGA_WHITE, VGA_BLACK));
    kprintf("           ___   ____\n");
    kprintf("     _____/   | / __ \\\n");
    kprintf("    / ___/ /| |/ /_/ /\n");
    kprintf("   / /  / ___ |\\__, /\n");
    kprintf("  / /  /_/  |_/____/\n");
    kprintf(" /_/ x86 kernel\n");
    kprintf("        By Divyanshu Malekar\n");
 
    terminalSetColour(vga_entryColour(VGA_LIGHT_GREY, VGA_BLACK));

    gdtInstall();
    pic_remap();      // mask all IRQs
    idtInstall();    // install IDT (lidt)

    asm volatile("sti");

    kbInit();

    sLoop();
}

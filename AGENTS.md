# rA9 — x86 Toy Kernel

32-bit bare-metal kernel for i686, multiboot/GRUB compatible. Entry: `_start` (boot/boot.s) → `kernel_main(uint32_t magic, uint32_t addr)` (kernel.c). Current features: GDT, IDT/PIC, VGA text terminal, PIT timer, minimal libc (string.h), kprintf.

## Dev Environment

- **Cross-compiler**: `i686-elf-gcc` / `i686-elf-as` (required; host GCC will fail)
- **Emulator**: `qemu-system-i386`
- **Bootloader**: GRUB (multiboot header in `boot/boot.s`)

## Build & Test

```bash
make          # builds rA9.bin (runs grub-file --is-x86-multiboot check)
make run      # boots in QEMU
make clean    # removes *.o and rA9.bin
```

No test suite, no linter configured.

## Project Layout

```
boot/           # Assembly entry, multiboot header, GDT flush
  boot.s        # _start, stack setup, calls kernel_main(magic, addr)
  gdt.s         # gdt_flush: lgdt + far jump + segment reload
  isr.s         # ISR/IRQ stubs + isr_stub_table + idtLoad
kernel/         # C kernel core
  gdt.c/h       # GDT init (3 entries: null, code 0x08, data 0x10)
  idt.c/h       # IDT install (lidt), gate encoding
  isr.c/h       # ISR/IRQ dispatch, PIC remap/unmask, EOI
  kprintf.c/h   # printf implementation
  tty.h         # Terminal interface
drivers/        # Hardware drivers
  io.h          # port I/O (inb/outb)
  pit.c/h       # PIT channel 0: pitInit, pitTicks, pitSleep (IRQ 0)
  tty.c         # VGA text buffer
  vga.h         # VGA constants
libc/           # Minimal freestanding libc
  include/string.h
  string/*.c    # memcpy, memset, strlen, memcmp, memmove
linker.ld       # Links at 1M, 4K-aligned sections
```

## Conventions

- **Naming**: snake_case for functions/variables (`gdt_install`, `terminal_put_string`)
- **Headers**: `#include "local.h"` for project, `#include <stdint.h>` for system
- **Attributes**: `__attribute__((packed))` on hardware-mapped structs (GDT, IDT, ISR frame)
- **Integer types**: Explicit `uint8_t`/`uint16_t`/`uint32_t` everywhere
- **Assembly**: AT&T syntax, `.global` exports, `.type @function`

## Pitfalls

- **`kernel_main` signature**: receives Multiboot magic (`eax`) and info pointer (`ebx`), pushed on the stack by `boot.s` before `call kernel_main`. PMM consumes these later.
- **IRQ EOI**: slave IRQ ≥8 sends `outb(0xA0, 0x20)` then master `outb(0x20, 0x20)` (slave cascades through IRQ2). Missing either hangs the PIC.
- **PIT must be initialized**: `pitInit(hz)` must be called before `pitTicks()`/`pitSleep()` — `ticks` only increments in the IRQ 0 handler.
- **Cross-compiler mandatory**: Building with host GCC produces broken binary (Makefile checks for `__i386__`).
- **Stack is 16 KiB** in `.bss` (boot.s:18-20) — avoid deep recursion.
- **No paging, no heap yet** — kernel_main runs in flat identity-mapped memory.
- **Object files (*.o) and *.bin are gitignored** — rebuild after clone. `objective.md` and `*.log` are also ignored.
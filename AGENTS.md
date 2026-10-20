# rA9 — x86 Toy Kernel

32-bit bare-metal kernel for i686, multiboot/GRUB compatible. Entry: `_start` (boot/boot.s) → `kernel_main()` (kernel.c). Current features: GDT, VGA text terminal, minimal libc (string.h), kprintf.

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
  boot.s        # _start, stack setup, calls kernel_main
  gdt.s         # gdt_flush: lgdt + far jump + segment reload
kernel/         # C kernel core
  gdt.c/h       # GDT init (3 entries: null, code 0x08, data 0x10)
  tty.h         # Terminal interface
  kprintf.c/h   # printf implementation
drivers/        # Hardware drivers
  io.h          # port I/O (inb/outb)
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
- **Attributes**: `__attribute__((packed))` on hardware-mapped structs (GDT)
- **Integer types**: Explicit `uint8_t`/`uint16_t`/`uint32_t` everywhere
- **Assembly**: AT&T syntax, `.global` exports, `.type @function`

## Pitfalls

- **GDT struct layout must match hardware exactly**: 8-byte descriptors. Current `gdt.h` has `baseMid` as `uint16_t` (should be `uint8_t`) — causes triple fault on `lgdt`/`ljmp`. See `kernel/gdt.c:12-17` for matching fixes.
- **Cross-compiler mandatory**: Building with host GCC produces broken binary (Makefile checks for `__i386__`).
- **Stack is 16 KiB** in `.bss` (boot.s:18-20) — avoid deep recursion.
- **No paging, no heap, no interrupts yet** — kernel_main runs in flat identity-mapped memory.
- **Object files (*.o) and *.bin are gitignored** — rebuild after clone.
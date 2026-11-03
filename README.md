# rA9

A small 32-bit x86 kernel that boots on bare metal (or in QEMU) via GRUB and the
Multiboot specification. It has no dependency on an existing OS: it brings up its
own GDT, IDT, interrupt controllers, VGA text console, PS/2 keyboard driver,
physical frame allocator, and an interactive shell.

---

## Requirements

The kernel **must** be built with a 32-bit ELF cross-compiler. A host compiler
will be rejected by the `#error` guards in `kernel.c`.

Required tools:

| Tool | Purpose |
| --- | --- |
| `i686-elf-gcc` | C compiler and linker driver |
| `i686-elf-as` | GNU assembler |
| `make` | Build orchestration |
| `grub-file` | Verifies the Multiboot header |
| `grub-mkrescue` | Creates the bootable ISO |
| `xorriso`, `mtools` | Used internally by `grub-mkrescue` |
| `qemu-system-i386` | Emulator / test target |

Verify they are available:

```bash
i686-elf-gcc --version
i686-elf-as --version
grub-file --version
grub-mkrescue --version
qemu-system-i386 --version
```

---

## Build and run

```bash
make          # compile everything and produce rA9.bin
make iso      # build the GRUB ISO (rA9.iso)
make run      # build the ISO and boot it in QEMU
make clean    # remove object files, rA9.bin, rA9.iso, and iso/
```


QEMU's default RAM for `qemu-system-i386` is 128 MiB. To test different memory
sizes, invoke QEMU directly:

```bash
qemu-system-i386 -m 512M -cdrom rA9.iso
```

---

## Project structure

```text
boot/
  boot.s
  gdt.s
  isr.s
kernel/
  kernel.c
  gdt.c / gdt.h
  idt.c / idt.h
  isr.c / isr.h
  pmm.c / pmm.h
  shell.c / shell.h
  kprintf.c / kprintf.h
  multiboot.h
  tty.h
drivers/
  tty.c
  vga.h
  keyboard.c/.h
  pit.c / pit.h
  io.h
libc/
  include/string.h
  string/*.c
linker.ld
Makefile
```

---

## Debugging

Run QEMU with logging:

```bash
qemu-system-i386 -cdrom rA9.iso -no-reboot -no-shutdown \
    -d int,cpu_reset -D qemu.log
```

Run QEMU with a GDB stub:

```bash
qemu-system-i386 -cdrom rA9.iso -s -S
```

In another terminal:

```bash
gdb rA9.bin
(gdb) target remote :1234
(gdb) break kernel_main
(gdb) continue
```

---


## References

- [OSDev Wiki](https://wiki.osdev.org/) — GDT, IDT, PIC, PIT, PS/2, VGA, paging.
- [Multiboot Specification](https://www.gnu.org/software/grub/manual/multiboot/)
- [Broken Thorn — OSDev 17: Physical Memory Manager](https://www.brokenthorn.com/Resources/OSDev17.html)
- [Brennan — Write a Shell in C](https://brennan.io/2015/01/16/write-a-shell-in-c/)
- [Embedded Artistry — memset, memcpy, memcmp, memmove](https://embeddedartistry.com/blog/2017/03/22/memset-memcpy-memcmp-and-memmove/)
- [Apple/XNU memcpy implementation](https://web.archive.org/web/20231010051517/https://opensource.apple.com/source/xnu/xnu-2050.7.9/libsyscall/wrappers/memcpy.c)

CROSS  := i686-elf-
CC     := $(CROSS)gcc
AS     := $(CROSS)as
CFLAGS := -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I. -Ilibc/include
SRC_C  := $(shell find . -name '*.c')
SRC_S  := $(shell find . -name '*.s')
OBJ    := $(SRC_C:.c=.o) $(SRC_S:.s=.o)

rA9.bin: $(OBJ) linker.ld
	$(CC) -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $(OBJ) -lgcc
	grub-file --is-x86-multiboot $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.s
	$(AS) $< -o $@

run: rA9.iso
	qemu-system-i386 -cdrom rA9.iso

iso: rA9.bin
	mkdir -p iso/boot/grub
	cp rA9.bin iso/boot/rA9.bin
	echo 'menuentry "rA9" { multiboot /boot/rA9.bin }' > iso/boot/grub/grub.cfg
	grub-mkrescue -o rA9.iso iso 2>/dev/null

clean:
	rm -f $(OBJ) rA9.bin rA9.iso
	rm -rf iso
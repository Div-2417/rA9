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

run: rA9.bin
	qemu-system-i386 -kernel rA9.bin

clean:
	rm -f $(OBJ) rA9.bin
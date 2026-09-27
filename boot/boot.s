# constants
.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

# multiboot header bootloader cheak first 8k bytes in kernel image[] for checking compatibility
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# we have made a empty stack of 16k that's grow bottom that's what i understand
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:
 
.section .text
.global _start
.type _start, @function
_start:
	# kernel now loaded by the bootloader

	# point esp register to top the stack
	mov $stack_top, %esp
	call kernel_main
	cli

1:	hlt
	jmp 1b

.size _start, . - _start

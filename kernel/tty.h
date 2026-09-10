#ifndef KERNEL_TTY_H
#define KERNEL_TTY_H

#include <stddef.h>
#include <stdint.h>

#include "../drivers/vga.h"
#include "../libc/include/string.h"

void InitTerminal(void);
void terminalPutCharAt(char c, enum VGAColour colour, size_t x, size_t y);
void terminalPutChar(char c);
void terminalPutString(const char* s);
void terminalSetColour(uint8_t colour);

#endif
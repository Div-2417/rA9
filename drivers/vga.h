#ifndef DRIVERS_VGA_H
#define DRIVERS_VGA_H

#include <stdint.h>

enum VGAColour {
    VGA_BLACK = 0,
    VGA_BLUE = 1,
    VGA_GREEN = 2,
    VGA_CYAN = 3,
    VGA_RED = 4,
    VGA_VIOLET = 5,
    VGA_BROWN = 6,
	VGA_LIGHT_GREY = 7,
	VGA_DARK_GREY = 8,
	VGA_LIGHT_BLUE = 9,
	VGA_LIGHT_GREEN = 10,
	VGA_LIGHT_CYAN = 11,
	VGA_LIGHT_RED = 12,
	VGA_LIGHT_MAGENTA = 13,
	VGA_LIGHT_BROWN = 14,
	VGA_WHITE = 15,

};

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

// i assume this function is used to merge fr and bg together
static inline uint8_t vga_entryColour(enum VGAColour fg, enum VGAColour bg){
    return (bg << 4) | fg;
}

//packs the character and colour into a single 16-bit value
static inline uint16_t vga_entry(unsigned char uc, uint16_t colour){
    return uc | (colour << 8);
}

#endif
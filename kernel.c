#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// cross compiler checks
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

enum VGAColour {
    VGA_BLACK = 0,
    VGA_BLUE = 1,
    VGA_GREEN = 2,
    VGA_CYAN = 3,
    VGA_RED = 4,
    VGA_VIOLET = 5,
    VGA_COLOR_BROWN = 6,
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
uint8_t vga_entryColour(enum VGAColour fg, enum VGAColour bg){
    return (bg << 4) | fg;
}

//packs the character and colour into a single 16-bit value
uint16_t vga_entry(unsigned char uc, uint16_t colour){
    return uc | (colour << 8);
}

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

//terminal state
size_t terminalRow, terminalColoumn;
uint8_t terminalColour;
uint16_t* terminalBuffer;

void InitTerminal(){
    terminalRow = 0;
    terminalColoumn = 0;
    terminalColour = vga_entryColour(VGA_LIGHT_GREY, VGA_BLACK);
    terminalBuffer = (uint16_t*)VGA_MEMORY;

    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x =0; x < VGA_WIDTH; x++) {
            size_t index = y*VGA_WIDTH + x;
            terminalBuffer[index] = vga_entry(' ', terminalColour);
        }
    }

}

void terminalPutCharAt(char c, enum VGAColour colour, size_t x, size_t y){
    size_t index = y*VGA_WIDTH + x;
    terminalBuffer[index] = vga_entry(c, colour);
   
}

void terminalPutChar(char c){
    terminalPutCharAt(c, terminalColour, terminalColoumn, terminalRow);

    if (++terminalColoumn == VGA_WIDTH) {
		terminalColoumn = 0;
		if (++terminalRow == VGA_HEIGHT)
			terminalRow = 0;
    }
}

void terminalPutString(const char* s){
    // not as dumb as gta online developers
    size_t length = strlen(s);

    for (size_t i = 0; i < length; i++){
        terminalPutChar(s[i]);
    }
}

void kernel_main(){
    InitTerminal();

    terminalPutString("hello MFs!!!");
}
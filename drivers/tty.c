#include <kernel/tty.h>

//terminal state
static size_t terminalRow, terminalColoumn;
static uint8_t terminalColour;
static uint16_t* terminalBuffer;

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

    if (c == '\n') {
        terminalColoumn = 0;
        if (++terminalRow == VGA_HEIGHT ) {
            terminalRow =0;
        }

        return;
    }

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

void terminalSetColour(uint8_t colour){
    terminalColour = colour;
}
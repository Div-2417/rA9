#include "keyboard.h"
#include "io.h"
#include "kernel/isr.h"

#include <stdint.h>

#define kbData 0x60
#define kbStatus 0x64
#define kbBufferSize 256

static volatile uint8_t kbBuffer[kbBufferSize];
static volatile uint32_t kbHead, kbTail;

#define SC_LCTRL       0x1D
#define SC_LSHIFT      0x2A
#define SC_RSHIFT      0x36
#define SC_CAPS        0x3A

static uint8_t shiftDown, ctrlDown, capsOn, extPrefix;

const char kbMap[128]={
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 0-9 */
    '9', '0', '-', '=', '\b', /* Backspace */
    '\t', /* Tab */
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, /* Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, /* Left Shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 
    0, /* Right Shift */
    '*', 
    0, /* Alt */
    ' ' /* Space bar */
};

static const char kbMapShift[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',   /* 0-9   */
    '(', ')', '_', '+', '\b',                         /* 10-14 */
    '\t',
    'Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
    0,                                                /* Ctrl  */
    'A','S','D','F','G','H','J','K','L',':','"','~',
    0,                                                /* LShift */
    '|','Z','X','C','V','B','N','M','<','>','?',
    0,                                                /* RShift */
    '*',
    0,                                                /* Alt   */
    ' '
};

struct event{
    uint8_t scancode;
    uint8_t pressed;
    uint8_t extended;
};

void kbPush(uint8_t c){
    uint32_t next = (kbHead + 1) % kbBufferSize;

    if (next != kbTail) {
        kbBuffer[kbHead]= c;
        kbHead = next; 
    }
}

int kbGetChar(void){
    if (kbHead == kbTail) {
        return -1;
    }

    uint8_t c = kbBuffer[kbTail];
    kbTail = (kbTail + 1) % kbBufferSize;

    return c;
}

char kbReadChar(void) {
    for (;;) {
        __asm__ volatile ("cli");
        int c = kbGetChar();
        if (c >= 0) {
            __asm__ volatile ("sti");
            return (char)c;
        }
        __asm__ volatile ("sti; hlt"); 
    }
}

int isDataAvailable(){
    return (inb(kbStatus)& 1);
}

int scTranslate(uint8_t scanCode){
    if (scanCode < 128) {
        return kbMap[scanCode];
    }
    
    return 0;
}

static void scProcess(uint8_t sc){
    if (sc == 0xE0) { extPrefix = 1; return; }

    if (extPrefix) {
        extPrefix = 0;
        return;
    }

    uint8_t released = sc & 0x80;
    uint8_t code     = sc & 0x7F;

    if (code == SC_LSHIFT || code == SC_RSHIFT) { shiftDown = !released; return; }
    if (code == SC_LCTRL)                       { ctrlDown  = !released; return; }
    if (code == SC_CAPS) { if (!released) capsOn ^= 1; return; }

    if (released) return;

    char c = kbMap[code];
    if (c == 0) return;

    int isLetter = (c >= 'a' && c <= 'z');

    if (ctrlDown && isLetter) {
        c = c & 0x1F;
    } else if (isLetter) {
        if (shiftDown ^ capsOn) c -= 32;
    } else if (shiftDown) {
        c = kbMapShift[code];
    }

    kbPush((uint8_t)c);
}

void kbHandler(void){
    if (isDataAvailable()) {
        scProcess(inb(kbData));
    }
}

void kbInit(void) {
    kbHead = kbTail = 0;
    shiftDown = ctrlDown = capsOn = extPrefix = 0;

    for (int i = 0; i < 64 && (inb(kbStatus) & 0x01); i++)
        (void)inb(kbData);

    register_interrupt_handler(1, kbHandler);

    outb(0x21, inb(0x21) & ~(1 << 1));
}
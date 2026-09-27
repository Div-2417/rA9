#include <kernel/kprintf.h>
#include <kernel/tty.h>
#include <stdarg.h>
#include <stddef.h>

static void printUnsigned(unsigned int value, unsigned int base) {
    char buf[32];
    const char* digits = "0123456789abcdef";
    int i = 0;

    if (value == 0) {
        terminalPutChar('0');
        return;
    }

    while (value > 0) {
        buf[i++] = digits[value % base];
        value /= base;
    }

    while (i > 0) {
        terminalPutChar(buf[--i]);
    }
}

static void printInt(int value) {
    if (value < 0) {
        terminalPutChar('-');
        value = -value;
    }
    printUnsigned((unsigned int)value, 10);
}

void kprintf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    for (size_t i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] != '%') {
            terminalPutChar(fmt[i]);
            continue;
        }

        i++; // skip '%'
        switch (fmt[i]) {
            case 'd': {
                int val = va_arg(args, int);
                printInt(val);
                break;
            }
            case 'x': {
                unsigned int val = va_arg(args, unsigned int);
                printUnsigned(val, 16);
                break;
            }
            case 'u': {
                unsigned int val = va_arg(args, unsigned int);
                printUnsigned(val, 10);
                break;
            }
            case 'c': {
                char c = (char)va_arg(args, int); // char promoted to int in varargs
                terminalPutChar(c);
                break;
            }
            case 's': {
                char* s = va_arg(args, char*);
                terminalPutString(s);
                break;
            }
            case '%':
                terminalPutChar('%');
                break;
            default:
                terminalPutChar('%');
                terminalPutChar(fmt[i]);
                break;
        }
    }

    va_end(args);
}
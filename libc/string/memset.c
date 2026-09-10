//improved version of memset with optimizations; last of this article
// https://embeddedartistry.com/blog/2017/03/22/memset-memcpy-memcmp-and-memmove/

#include <string.h>
#include <stddef.h>

void* memset(void* desptr, int c, size_t n){
    unsigned char* s = desptr;

    while (n>0) {
        *s = (unsigned char) c;
        s++;
        n--;
    }

    return desptr;
}


// optimizations for individual memcpy accesses; makes large transfer faster
// https://web.archive.org/web/20231010051517/https://opensource.apple.com/source/xnu/xnu-2050.7.9/libsyscall/wrappers/memcpy.c

#include <string.h>
#include <stddef.h>

void *memcpy(void *desptr, const void *srcptr, size_t n){
    unsigned char *des = desptr;
	const unsigned char *src = srcptr;

    //size 0 no data to be copied
    if (n == 0)
        return desptr;

    //copying byte by byte
    size_t i;
    for (i =0; i<n; i++){
        des[i] = src[i];
    }

    return desptr;
}
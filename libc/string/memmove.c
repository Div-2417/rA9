#include <string.h>
#include <stddef.h>

void *memmove(void *desptr, const void *srcptr, size_t n){
    unsigned char *des = desptr;
	const unsigned char *src = srcptr;
    size_t i;

    // des before src in memory, copy front to back
    if (des < src)
        for (i =0; i<n; i++)
            des[i] = src[i];
    else 
    // des after src copy back to front
        for (i =n; i> 0; i--)
            des[i-1] = src[i-1];

    return desptr;
}
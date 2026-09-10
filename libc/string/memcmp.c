#include <stdint.h>
#include <string.h>

int memcmp(const void *p1, const void *p2, size_t n){
    size_t i;

   //same memory;exit
    if (p1 == p2)
        return 0;

    //byte by byte comparision    
    for (i=0; i<n; i++)
        if(*(uint8_t *)p1 == *(uint8_t *)p2)
            p1 = 1 + (uint8_t *)p1, p2 = 1 + (uint8_t *)p2;
        else
            break;
        
    return (i == n) ? 0 : (*(uint8_t *)p1 - *(uint8_t *)p2);
}
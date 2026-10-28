#ifndef STRING_H
#define STRING_H

#include <stddef.h>

void* memcpy(void* desptr, const void* srcptr, size_t n);
void* memset(void* desptr, int c, size_t n);
void* memmove(void* desptr, const void* srcptr, size_t n);
size_t strlen(const char* s);
int memcmp(const void* p1, const void* p2, size_t n);
int strcmp(const char *s1, const char *s2);

#endif

// Reference to the guide used of PMM implement
// using the BitMap method
// https://www.brokenthorn.com/Resources/OSDev17.html

#ifndef KERNEL_PMM_H
#define KERNEL_PMM_H

#include <stdint.h>
#include <stddef.h>

#define pmmBlockPerByte 8
#define pmmBlockSize 4096
#define pmmBlockAlign pmmBlockSize

typedef uint32_t physicalAddr;

void  pmmInit(size_t memSizeKB, physicalAddr bitmap);
void  pmmInitRegion(physicalAddr base, size_t size);
void  pmmDeInitRegion(physicalAddr base, size_t size);
void* pmmAllocBlock(void);
void  pmmFreeBlock(void* p);

size_t   pmmGetMemSize(void);
uint32_t pmmGetBlockCount(void);
uint32_t pmmGetUsedBlockCount(void);
uint32_t pmmGetFreeBlockCount(void);
uint32_t pmmGetBitmapSize(void);
physicalAddr pmmGetBitmapAddr(void);

#endif

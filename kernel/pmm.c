#include "pmm.h"
#include "multiboot.h"
#include <stdint.h>
#include <stddef.h>

static uint32_t  memorySize = 0;
static uint32_t  memoryUsedBlocks = 0;
static uint32_t  memoryMaxBlocks  = 0;
static uint32_t* memoryMap  = 0;

static inline void mMapSet(int bit) {
    memoryMap[bit/32] |= (1u << (bit % 32));
}

static inline void mMapUnset(int bit) {
    memoryMap[bit/32] &= ~(1u << (bit % 32));
}

static inline int mMapTest(int bit) {
    return (memoryMap[bit/32] & (1u << (bit % 32))) != 0;
}

static int mMapFirstFree(void) {
    for (uint32_t i = 0; i < pmmGetBlockCount() / 32; i++) {
        if (memoryMap[i] != 0xFFFFFFFFu) {
            uint32_t bit = (uint32_t)__builtin_ctz(~memoryMap[i]);
            return (int)(i * 32 + bit);
        }
    }
    return -1;
}

void pmmInit(size_t memSizeKB, physicalAddr bitmap) {
    memorySize = memSizeKB * 1024;
    memoryMaxBlocks = (memSizeKB * 1024) / pmmBlockSize;
    memoryMap = (uint32_t*)bitmap;
    memoryUsedBlocks = memoryMaxBlocks;

    uint32_t mapWords = (memoryMaxBlocks + 31) / 32;
    for (uint32_t i = 0; i < mapWords; i++) {
        memoryMap[i] = 0xFFFFFFFFu;
    }
}

void pmmInitRegion(physicalAddr base, size_t size) {
    int align = base / pmmBlockSize;
    int blocks = size / pmmBlockSize;

    for (; blocks > 0; blocks--) {
        mMapUnset(align++);
        memoryUsedBlocks--;
    }
}

void pmmDeInitRegion(physicalAddr base, size_t size) {
    int align = base / pmmBlockSize;
    int blocks = size / pmmBlockSize;

    for (; blocks > 0; blocks--) {
        mMapSet(align++);
        memoryUsedBlocks++;
    }
}

void* pmmAllocBlock(void) {
    if (pmmGetFreeBlockCount() == 0) return 0;

    int frame = mMapFirstFree();
    if (frame == -1) return 0;

    mMapSet(frame);
    memoryUsedBlocks++;

    return (void*)((physicalAddr)frame * pmmBlockSize);
}

void pmmFreeBlock(void* p) {
    physicalAddr addr = (physicalAddr)p;
    int frame = addr / pmmBlockSize;

    mMapUnset(frame);
    memoryUsedBlocks--;
}

size_t   pmmGetMemSize(void) {
    return memorySize;
}

uint32_t pmmGetBlockCount(void) {
    return memoryMaxBlocks;
}

uint32_t pmmGetUsedBlockCount(void) {
    return memoryUsedBlocks;
}

uint32_t pmmGetFreeBlockCount(void) {
    return memoryMaxBlocks - memoryUsedBlocks;
}

physicalAddr pmmGetBitmapAddr(void) {
    return (physicalAddr)memoryMap;
}

uint32_t pmmGetBitmapSize(void) {
    return (memoryMaxBlocks + 7) / 8;
}
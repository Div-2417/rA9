#include "pmm.h"
#include <stdint.h>

static uint32_t  memorySize = 0;
static uint32_t  memoryUsedBlocks = 0;
static uint32_t  memoryMaxBlocks  = 0;
static uint32_t* memoryMap  = 0;

inline void mMapSet(int bit){
  memoryMap[bit/32] |= (1<< (bit % 32));
}

inline void mMapUnset(int bit){
  memoryMap[bit/32] &= ~(1<< (bit % 32));
}

inline int mMapTest(int bit){
  return (memoryMap[bit/32] & (1 << (bit % 32))) != 0;
}

static int mMapFirstFree(){
  for(uint32_t i=0; i< pmmGetBlockCount() /32;i++){
    if (memoryMap[i] != 0xFFFFFFFF) {
      uint32_t bit = (uint32_t)__builtin_ctz(~memoryMap[i]);
      return (int)(i * 32 + bit);
    }
  }

  return -1;
}

void pmmInit(size_t memSizeKB, physicalAddr bitmap){

  memorySize = memSizeKB * 1024;
  memoryMaxBlocks = memSizeKB / pmmBlockSize;
  memoryMap = (uint32_t*)bitmap;
  memoryUsedBlocks = memoryMaxBlocks;

  for (uint32_t i = 0; i < memoryMaxBlocks / 32; i++) {
    memoryMap[i] = 0xFFFFFFFF;
  }
}

void pmmDeInitRegion(physicalAddr base, size_t size){

  int align = base/pmmBlockSize;
  int blocks = size/pmmBlockSize;

  for(;blocks > 0; blocks--){
    mMapSet(align++);
    memoryUsedBlocks++;
  }
}

void* pmmAllocBlock(void){
  if (pmmGetFreeBlockCount() == 0) return 0;

  int frame = mMapFirstFree();
  if (frame == -1) return 0;

  mMapSet(frame);
  memoryUsedBlocks++;

  return (void*)((physicalAddr) frame* pmmBlockSize);
}

void pmmFreeBlock(void* p){
  physicalAddr addr = (physicalAddr)p;
  int frame = addr/pmmBlockSize;

  mMapUnset(frame);
  memoryUsedBlocks--;
}

size_t   pmmGetMemSize(void){
  return memorySize;
}

uint32_t pmmGetBlockCount(void){
  return memoryMaxBlocks;
}
uint32_t pmmGetUsedBlockCount(void){
  return memoryUsedBlocks;
}

uint32_t pmmGetFreeBlockCount(void){
  return memoryMaxBlocks - memoryUsedBlocks;
}

physicalAddr pmmGetBitmapAddr(void){
  return (physicalAddr)memoryMap;
}

uint32_t pmmGetBitmapSize(void){
  return (memoryMaxBlocks + pmmBlockPerByte - 1) / pmmBlockPerByte;
}

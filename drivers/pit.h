#ifndef DRIVERS_PIT_H
#define DRIVERS_PIT_H

#include <stdint.h>

void pitInit(uint32_t hz);
uint32_t pitTicks(void);
void pitSleep(uint32_t ms);

#endif

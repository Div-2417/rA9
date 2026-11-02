#ifndef KERNEL_SHELL_H
#define KERNEL_SHELL_H

#include <stdint.h>
#include "multiboot.h"

void sLoop(void);

extern multiboot_info_t* g_multiboot_info;

#endif
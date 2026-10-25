#ifndef DRIVERS_KEYBOARD_H
#define DRIVERS_KEYBOARD_H

#pragma once
void kbInit(void);
void kbHandler(void);
int  kbGetChar(void);
char kbReadChar(void);

#endif
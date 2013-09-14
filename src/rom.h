#ifndef ROM_H_
#define ROM_H_

#include "types.h"

// visibility area E000-FFFF
byte kernalROM[0x2000];

// visibility area A000-BFFF
byte basicROM[0x2000];

// visibility area D000-DFFF
byte characterROM[0x1000];


void loadROM();

#endif /*ROM_H_*/

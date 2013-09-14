#ifndef MOS6510_H_
#define MOS6510_H_

//#include <stdio.h>
//#include <stdlib.h>
#include "types.h"
#include "ram.h"
//#include "rom.h"
#include "testing.h"

void mos6510_init();
void mos6510_cycle();
void mos6510_HWInterrupt();

#endif /*MOS6510_H_*/

#ifndef IOREGAREA_H_
#define IOREGAREA_H_

#include "types.h"


byte ioAreaReadByte(word address);

void ioAreaWriteByte(word address, byte data);

void initIoRegArea();

void cycleIoRegArea();

#endif /*IOREGAREA_H_*/

/*
 ============================================================================
 Name        : ioregarea.h
 Author      : Sampo Peltonen
 Licence     : GNU General Public License, version 2.
 Copyright   : Copyright (C) 2013  Sampo Peltonen
 Description : Commodore 64 simulation
 ============================================================================
 */

#ifndef IOREGAREA_H_
#define IOREGAREA_H_

#include "types.h"


byte ioAreaReadByte(word address);

void ioAreaWriteByte(word address, byte data);

void initIoRegArea();

void cycleIoRegArea();

#endif /*IOREGAREA_H_*/

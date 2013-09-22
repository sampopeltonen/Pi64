/*
 ============================================================================
 Name        : MOS6510.h
 Author      : Sampo Peltonen
 Licence     : GNU General Public License, version 2.
 Copyright   : Copyright (C) 2013  Sampo Peltonen
 Description : MOS6510 simulation
 ============================================================================
 */

#ifndef MOS6510_H_
#define MOS6510_H_

#include "types.h"
#include "ram.h"

void mos6510_init();
void mos6510_cycle();
void mos_irq();
void mos_nmi();

#endif /*MOS6510_H_*/

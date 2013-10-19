/*
 ============================================================================
 Name        : VIC6569.h
 Author      : Sampo Peltonen
 Licence     : GNU General Public License, version 2.
 Copyright   : Copyright (C) 2013  Sampo Peltonen
 Description : VIC6569 simulation
 ============================================================================
 */

#ifndef VIC6569_H_
#define VIC6569_H_

//byte vicRegisters[0x40];
void vic6569_writeByte(byte address, byte data);
byte vic6569_readByte(byte address);

#endif /*VIC6569_H_*/

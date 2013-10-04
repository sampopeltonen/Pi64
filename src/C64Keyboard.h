/*
 ============================================================================
 Name        : C64Keyboard.h
 Author      : Sampo Peltonen
 Licence     : GNU General Public License, version 2.
 Copyright   : Copyright (C) 2013  Sampo Peltonen
 Description : Commodore 64 simulation
 ============================================================================
 */

#ifndef C64KEYBOARD_H_
#define C64KEYBOARD_H_

#include "types.h"

void C64Keyboard_init();

void C64Keyboard_writeValue(byte value);

byte C64Keyboard_readValue();

#endif /*C64KEYBOARD_H_*/

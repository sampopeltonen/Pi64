/*
 ============================================================================
 Name        : C64Keyboard.c
 Author      : Sampo Peltonen
 Licence     : GNU General Public License, version 2.
 Copyright   : Copyright (C) 2013  Sampo Peltonen
 Description : Commodore 64 simulation
 ============================================================================
 */

#include "C64Keyboard.h"
#include "stdlibtools.h"

#include "types.h"

// Note that pin numbering is also known in C64KeyboardPins.s
#define DATA_IN 22	// connected to 165-pin#9 (Qh) (serial data comes from in here)
#define LATCHPIN 23	// connectod to 595-pin#12 and 165-pin#15 (toggles modes)
#define CLOCKPIN 24	// connected to 595-pin#11 and 165-pin#2 (clock to make them tick)
#define SERLOADPIN 25	// connected to 595-pin#14 (serial data goes to here) and 165-pin#1 (SH/LD) (loads parallel data to internal register)

extern void initKeyboardGPIOPins();				// C64KeyboardPins.s
extern void serialDelay();					// C64KeyboardPins.s
extern void SetGpio(unsigned int pin, unsigned int value);	// gpio.s
extern unsigned int GetGpio(unsigned int pin);			// gpio.s

int lastOperationRead = 0;
byte lastReadValue=0;

void C64Keyboard_writeValue(byte value) {
	int i;
	int bitscan = 1;

	//SetGpio(LATCHPIN, 0);
	
	for(i=0; i<8; i++) {
		// Seems to work even without extra delays (serialDelay calls).
		SetGpio(CLOCKPIN, 0);
		serialDelay();
		SetGpio(SERLOADPIN,(value & bitscan));
		serialDelay();
		bitscan =bitscan<<1;
		SetGpio(CLOCKPIN, 1);
		serialDelay();
	}
	SetGpio(LATCHPIN, 1);
	lastOperationRead=0;
}

byte C64Keyboard_readValue() {
	if(lastOperationRead) return lastReadValue;
	int i;
	byte value=0;
	//SetGpio(LOADPIN, 0);
	SetGpio(SERLOADPIN, 0);
	serialDelay();
	SetGpio(CLOCKPIN, 0);
	serialDelay();
	SetGpio(CLOCKPIN, 1);
	serialDelay();
	//SetGpio(LOADPIN, 1);
	SetGpio(SERLOADPIN, 1);
	serialDelay();
	SetGpio(LATCHPIN, 0);
	serialDelay();
	for(i=0; i<8; i++) {
		if(i>0) value = value << 1;
		value |= GetGpio(DATA_IN);
		serialDelay();
		SetGpio(CLOCKPIN, 0);
		serialDelay();
		SetGpio(CLOCKPIN, 1);
		serialDelay();
	}
	lastOperationRead=1;
	lastReadValue=value;
	return value;
}



void C64Keyboard_init() {
	initKeyboardGPIOPins();
}

